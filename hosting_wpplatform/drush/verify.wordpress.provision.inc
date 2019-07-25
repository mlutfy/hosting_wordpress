<?php

/**
 * 
 */
function drush_provision_wordpress_verify_validate() {
}

/**
 * Implements the provision-wordpress-verify command.
 */
function drush_provision_wordpress_verify() {
  drush_log('WordPress: starting drush_provision_wordpress_verify', 'ok');

  // d()->service_subscribe('platform', d()->wpplatform);
  d()->service_subscribe('server', '@server_master');
  d()->service_subscribe('http', '@server_master');
  d()->service_subscribe('db', '@server_127.0.0.1');

  // FIXME .. this relies on something something that might be removed
  // from provision/db/Provision/Service/db.php.
  //
  // However, since we cloak the db setings in wp-config.php, and that
  // is also the simplest way to do multi-platform, ideally we should
  // have a proper Aegir API to fetch the DB credentials.
  $_SERVER['db_type'] = drush_get_option('db_type', NULL, 'site');
  $_SERVER['db_host'] = drush_get_option('db_host', NULL, 'site');
  $_SERVER['db_port'] = drush_get_option('db_port', NULL, 'site');
  $_SERVER['db_passwd'] = drush_get_option('db_passwd', NULL, 'site');
  $_SERVER['db_name'] = drush_get_option('db_name', NULL, 'site');
  $_SERVER['db_user'] = drush_get_option('db_user', NULL, 'site');
  $_SERVER['wp_content_dir'] = d()->root . '/sites/' . d()->uri . '/wp-content';

  // This makes the assumption that if LetsEncrypt is enabled on the web server,
  // the site will very soon be https-enabled.
  $server = d()->wpplatform->web_server;

  if (($server->Certificate_service_type == 'LetsEncrypt')) {
    $_SERVER['wp_content_url'] = 'https://' . d()->uri . '/sites/' . d()->uri . '/wp-content/';
  }
  else {
    $_SERVER['wp_content_url'] = 'http://' . d()->uri . '/sites/' . d()->uri . '/wp-content/';
  }

  // These options were loaded from the 'site' context, make sure
  // they get set in the 'wpsite' context, for the drushrc.php refresh.
  drush_set_option('db_type', $_SERVER['db_type'], 'wpsite');
  drush_set_option('db_host', $_SERVER['db_host'], 'wpsite');
  drush_set_option('db_port', $_SERVER['db_port'], 'wpsite');
  drush_set_option('db_passwd', $_SERVER['db_passwd'], 'wpsite');
  drush_set_option('db_name', $_SERVER['db_name'], 'wpsite');
  drush_set_option('db_user', $_SERVER['db_user'], 'wpsite');
  drush_set_option('wp_content_dir', $_SERVER['wp_content_dir'], 'wpsite');
  drush_set_option('wp_content_url', $_SERVER['wp_content_url'], 'wpsite');

  // Refresh the drushrc.php
  $config = new Provision_Config_Drushrc_wpsite(d()->name);
  $config->write();

  // Directory permissions
  provision_wordpress_setup_directories();

  d()->service('http')->restart();

  drush_log('WordPress: finished drush_provision_wordpress_verify', 'ok');
}
