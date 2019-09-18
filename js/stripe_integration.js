/**
 * @file
 * Frontend logic for Stripe integration.
 */

(function ($) {
  'use strict';

  Drupal.behaviors.bcshop_stripe = {
    attach: function (context, settings) {
      var stripe_settings = Drupal.settings.stripe;
      $('#bcshop-stripe-redirect-form').submit(function (event) {
        event.preventDefault();

        var stripe = Stripe(stripe_settings.pk);

        stripe.redirectToCheckout({
          sessionId: stripe_settings.sid
        }).then(function (result) {
          // If `redirectToCheckout` fails due to a browser or network
          // error, display the localized error message to your customer
          // using `result.error.message`.
        });

      });
    }
  };
})(jQuery);
