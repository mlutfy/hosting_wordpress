<?php

/**
 * Check that we are trying to install a new site, and a new site only
 */
function drush_provision_wordpress_install_validate() {
  drush_log('WordPress: starting drush_provision_wordpress_install_validate', 'ok');

  if (!d()->uri) {
    return drush_set_error("PROVISION_URL_REQUIRED", dt("You need to specify a valid url to install a site"));
  }

/* TODO
  if (_provision_drupal_site_exists()) {
    return drush_set_error('PROVISION_SITE_INSTALLED');
  }
*/

  drush_log('WordPress: finished drush_provision_wordpress_install_validate', 'ok');
}

/**
 * Implements the provision-wordpress-install command.
 *
 * Mostly copies /usr/share/drush/commands/provision/platform/reset.login.provision.inc
 * but for uid = 2.
 */
function drush_provision_wordpress_install() {
  drush_log('WordPress: starting drush_provision_wordpress_install', 'ok');

  // d()->service_subscribe('platform', d()->wpplatform);
  d()->service_subscribe('server', '@server_master');
  d()->service_subscribe('http', '@server_master');
  d()->service_subscribe('db', '@server_127.0.0.1');

  d()->service('db')->create_site_database();
  d()->service('http')->create_config('site');

  drush_log('WordPress: finished drush_provision_wordpress_install', 'ok');
}
