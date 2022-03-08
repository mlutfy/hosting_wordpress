<?php

/**
 * @file
 * Implementation for provision-wordpress-import
 * Based on provision/platform/import.provision.inc
 */

/**
 * Import the information about the existing site, and return it in the $data context array
 */
function drush_provision_wordpress_import() {
  if (d()->type === 'wpsite') {
    drush_set_option('installed', TRUE, 'wpsite');
  }
}

function drush_provision_wordpress_post_provision_wordpress_import() {
  // None of this seems really necessary. Mostly copied from drupal-import
  if (d()->type === 'wpsite') {
    // d()->aliases = drush_set_option('aliases', provision_drupal_find_aliases());
    // d()->write_alias();

    // set this in the site options too so it recurrs.
    // drush_set_option('aliases', drush_get_option('aliases'),  'site');

    // generate the drushrc
    // provision_save_site_data();

    // Do not automatically save the drushrc at the end of the command.
    // drush_set_option('provision_save_config', false);

    // We still run this extra backend sub-task to properly register packages etc.
    provision_backend_invoke(d()->name, 'provision-wordpress-verify');
    sleep(5); // A small trick to avoid high load and race conditions.

    // We run this extra verify via frontend to avoid issue #1004526.
    provision_backend_invoke('@hostmaster', 'hosting-task', array(d()->name, 'wordpress-verify'), array('force' => TRUE));

    provision_reload_config('wpsite');
  }
}
