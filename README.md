## FEATURES

The module implements full Stripe Checkout integration as an offsite payment method.


## REQUIREMENTS

Requires the stripe/stripe-php library 


## HOW TO MAKE IT WORK

The stripe/stripe-php library needs to be placed in the site libraries folder.

Currently there is no configuration page for the module, API keys need to be set either
in settings.php or using variable_set when executing PHP code or drush:

```
$conf['commerce_stripe_environment_settings'] = array(
  'TEST' => array(
    'secret_key' => 'sk_test_***',
    'public_key' => 'pk_test_***',
  ),
  'LIVE' => array(
    'secret_key' => 'sk_live_***',
    'public_key' => 'pk_live_***',
  ),
);
```


## TODO

* Update module namespace
* Test
* Implement a config page where one can set values of test and live API keys.
* Implement Checkout in an iframe.
* Implement other integration types.
