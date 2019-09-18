<?php

/**
 * @file
 * Contains Drupal Commerce payment method callbacks.
 */

use Stripe\Stripe;
use Stripe\Checkout\Session as StripeSession;

/**
 * Redirect form callback.
 */
function bcshop_stripe_redirect_form($form, &$form_state, $order, $payment_method) {

  libraries_load('stripe-php');

  $stripe_keys = _bcshop_stripe_get_api_keys();

  // Initialize Stripe and create a session.
  Stripe::setApiKey($stripe_keys['secret_key']);

  // Get line items data.
  $session_line_items = [];
  $line_item_ids = [];
  $total_amount = 0;
  foreach (field_get_items('commerce_order', $order, 'commerce_line_items') as $item) {
    $line_item_ids[] = $item['line_item_id'];
  }
  $line_items = commerce_line_item_load_multiple($line_item_ids);
  foreach ($line_items as $id => $line_item) {
    if (!empty($line_item->commerce_product[LANGUAGE_NONE])) {
      $product = commerce_product_load($line_item->commerce_product[LANGUAGE_NONE][0]['product_id']);
      $currency_code = strtolower($line_item->commerce_total[LANGUAGE_NONE][0]['currency_code']);
      $session_line_items[] = [
        'name' => $product->title,
        'images' => [],
        'amount' => $line_item->commerce_total[LANGUAGE_NONE][0]['amount'],
        'currency' => $currency_code,
        'quantity' => intval($line_item->quantity),
      ];
      $total_amount += $line_item->commerce_total[LANGUAGE_NONE][0]['amount'];
    }
  }

  // Create a payment transaction and save it to get its ID.
  $transaction = commerce_payment_transaction_new('bcshop_stripe', $order->order_id);
  $transaction->amount = $total_amount;
  $transaction->currency_code = $currency_code;
  $transaction->status = COMMERCE_PAYMENT_STATUS_PENDING;
  commerce_payment_transaction_save($transaction);

  // Get metadata.
  $metadata = module_invoke_all('bcshop_stripe_metadata', $order);
  if (count($metadata)) {
    $session['payment_intent_data']['metadata'] = $metadata;
  }

  // Create Stripe session.
  $session = StripeSession::create([
    'mode' => 'payment',
    'submit_type' => 'pay',
    'payment_method_types' => ['card'],
    'payment_intent_data' => [
      'metadata' => $metadata,
    ],
    'customer_email' => $order->mail,
    'line_items' => $session_line_items,
    'success_url' => $GLOBALS['base_url'] . '/bcshop-stripe/payment-return/' . $transaction->transaction_id . '/success',
    'cancel_url' => $GLOBALS['base_url'] . '/bcshop-stripe/payment-return/' . $transaction->transaction_id . '/failure',
  ]);

  // Save the payment transaction with the checkout session ID.
  $transaction->remote_id = $session->id;
  $transaction->data['payment_intent_id'] = $session->payment_intent;
  commerce_payment_transaction_save($transaction);

  $form['bcshop_stripe'] = [
    '#attached' => [
      'js' => [
        [
          'data' => [
            'stripe' => [
              'pk' => $stripe_keys['public_key'],
              'sid' => $session->id,
            ],
          ],
          'type' => 'setting',
        ],
      ],
      'library' => [
        ['bcshop_stripe', 'stripe-front'],
      ],
    ],
  ];

  return $form;

}

/**
 * Stripe return callback.
 */
function bcshop_stripe_process_return($commerce_payment_transaction, $status = 'failure') {
  $order = commerce_order_load($commerce_payment_transaction->order_id);
  if ($status === 'success') {
    _bcshop_stripe_check_transaction_status($commerce_payment_transaction);
  }
  else {
    drupal_set_message(t('Payment failed.'), 'error');
    // If failed we don't need this transaction anymore.
    commerce_payment_transaction_delete($commerce_payment_transaction->transaction_id);
    commerce_payment_redirect_pane_previous_page($order);
  }
  drupal_goto('checkout/' . $order->order_id);
}
