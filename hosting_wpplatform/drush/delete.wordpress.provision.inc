<?php

/**
 * Implements the provision-wordpress-delete command.
 */
function drush_provision_wordpress_delete() {
  if (d()->type === 'wpsite') {
    d()->service('db')->destroy_site_database();

    // Taken from ./platform/delete.provision.inc (todo: alias symlinks and client symlinks?)
    drush_invoke_process('@none', 'provision-save', array(d()->name), array('delete' => TRUE));

/*
    foreach (d()->drush_aliases as $alias) {
      drush_invoke_process('@none', 'provision-save', array($alias), array('delete' => TRUE));
    }
*/

    // Do not automatically save the drushrc at the end of the command.
    drush_set_option('provision_save_config', FALSE);

    // Deletes the contents of ~/platforms/foo/sites/example.org
    _provision_recursive_delete(d()->site_path);

    // Deletes the web server vhost config file.
    d()->service('http')->delete_config('site');
  }
}
