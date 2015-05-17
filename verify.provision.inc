<?php

/**
 * Implements hook_provision_verify().
 */
function drush_provision_wordpress_provision_verify($url = NULL) {
  if (d()->type == 'wpplatform') {
    drush_log(dt("CiviCRM: Running drush_wordpress_provision_verify on !type, name !name", array('!type' => d()->type, '!name' => d()->name)));

    provision_file()->exists(d()->root . '/wp-includes')
      ->succeed('Directory @path seems to be a WordPress installation.')
      ->fail('Directory @path does not seem to be a valid Wordpress installation (missing wp-includes).', 'PROVISION_WORDPRESS_INVALID_ROOT');

    d()->service_subscribe('server', '@server_master');
    d()->service_subscribe('http', '@server_master');
    d()->service_subscribe('wpplatform', d()->name);

    // This causes PHP to segfault:
    // d()->service('wpplatform')->init_wpplatform();
    // d()->service('wpplatform')->create_config('wpplatform');

    $config = new Provision_Config_wpplatform(d()->name);
    $config->write();
  }
}
