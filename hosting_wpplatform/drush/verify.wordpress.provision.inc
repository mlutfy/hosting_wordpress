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

  // d()->service('db')->create_site_database();
  // d()->service('http')->create_config('site');

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
  $_SERVER['wp_content_dir'] = drush_get_option('wp_content_dir', NULL, 'site');

  // Directory permissions
  provision_wordpress_setup_directories();

  d()->service('http')->restart();

  drush_log('WordPress: finished drush_provision_wordpress_verify', 'ok');
}
