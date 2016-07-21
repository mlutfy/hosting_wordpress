<?php

/**
 * Implements the provision-wordpress-enable command.
 */
function drush_provision_wordpress_enable() {
  drush_log('WordPress: starting drush_provision_wordpress_enable', 'ok');

  d()->service('http')->create_config('site');
  d()->service('http')->parse_configs();

  drush_log('WordPress: finished drush_provision_wordpress_enable', 'ok');
}
