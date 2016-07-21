<?php

/**
 * Implements the provision-wordpress-disable command.
 */
function drush_provision_wordpress_disable() {
  drush_log('WordPress: starting drush_provision_wordpress_disable', 'ok');

  d()->site_enabled = FALSE;
  d()->setProperty('site_enabled', FALSE);
  drush_set_option('site_enabled', FALSE);

  d()->write_alias();

  // FIXME According to provision/http/disable.provision.inc,
  // this is not a good way of disabling a site.
  d()->service('http')->create_config('site');
  d()->service('http')->parse_configs();

  drush_log('WordPress: finished drush_provision_wordpress_disable', 'ok');
}
