<?php

/**
 * @file
 * Contains administrative settings for the module.
 */

/**
 * Settings form builder.
 */
function bschop_stripe_settings_form($form, &$form_state) {

  $currencies = commerce_currencies(TRUE);
  $currency_list = [];
  $supported_currencies = commerce_stripe_payment_currencies();

  // Ensure we only include currencies we support.
  foreach ($currencies as $currency_code => $currency) {
    if (in_array(strtolower($currency_code), $supported_currencies)) {
      $currency_list[$currency_code] = $currency['name'];
    }
  }

  $form['bschop_stripe_currency'] = [
    '#type' => 'select',
    '#title' => t('Currency'),
    '#options' => $currency_list,
    '#description' => t('Select the currency that you are using.'),
  ];
  $form['bschop_stripe_secret_key'] = [
    '#type' => 'textfield',
    '#title' => t('Secret Key'),
    '#description' => t('Secret API Key. Get your key from https://stripe.com/'),
    '#required' => TRUE,
  ];
  $form['bschop_stripe_public_key'] = [
    '#type' => 'textfield',
    '#title' => t('Publishable Key'),
    '#description' => t('Publishable API Key. Get your key from https://stripe.com/'),
    '#required' => TRUE,
  ];

  foreach ($form as $key => $element) {
    $form[$key]['#default_value'] = variable_get($key, '');
  }

  return system_settings_form($form);
}
