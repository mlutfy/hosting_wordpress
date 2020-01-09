<?php

/**
 * Implements the provision-wordpress-cli command.
 */
function drush_provision_wordpress_cli() {
  // Required so that wp-cli finds the civicrm package
  // however, right now we have symlinks for civicrm to a common place,
  // but WordPress now has better ways of dealing with this.
  $_SERVER['wp_content_dir'] = d()->root . '/sites/' . d()->uri . '/wp-content';

  // Rebuild a new $args array with the arguments after the 'wp' arg.
  // Normally argv will have:
  // 0 = /path/to/drush.php
  // 1 = --php=foo
  // 2 = --php-options=bar
  // 3 = @fooalias
  // 4 = wp
  // 5 = info
  // 6 = --help (etc)
  $args = [];
  $found = false;

  foreach ($GLOBALS['argv'] as $arg) {
    if ($found) {
      $args[] = escapeshellarg($arg);
    }

    // On the next iteration, start saving arguments into $args
    if ($arg == 'wp') {
      $found = true;
    }
  }

  if (empty($args)) {
    drush_set_error("HOSTING_WPPLATFORM_CLI_ERROR", dt("Could not parse the command line arguments, 'wp' not found."));
    return;
  }

  $args = implode(' ', $args);
  provision_wordpress_wpcli_run($args);
}
