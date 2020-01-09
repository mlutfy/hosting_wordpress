<?php

/**
 * Directory/file checks for install and verify.
 */
function provision_wordpress_setup_directories() {
  $wp_content_dir = d()->site_path . '/wp-content';

  $content_dirs = array(
    $wp_content_dir,
    $wp_content_dir . '/plugins',
    $wp_content_dir . '/themes',
    $wp_content_dir . '/uploads',
  );

  foreach ($content_dirs as $dir) {
    $exists = provision_file()->exists($dir)
      ->succeed('WordPress: @path exists.')
      ->status();

    if (! $exists) {
      $exists = provision_file()->mkdir($dir)
        ->succeed('Wordpress: @path has been created.')
        ->fail('CiviCRM: @path could not be created.', 'DRUSH_PERM_ERROR')
        ->status();
    }

    // setgid directories, it avoids a few permissions issues
    // where files are aegir.www-data.
    provision_file()->chmod($dir, 02770, TRUE)
      ->succeed('Changed permissions of @path to @perm')
      ->fail('Could not change permissions <code>@path to @perm')
      ->status();

    provision_file()->chgrp($dir, d('@server_master')->web_group, TRUE)
      ->succeed('Changed group ownership @path to @gid')
      ->fail('Could not change group ownership of @path to @gid')
      ->status();
  }

  // Symlink the twentyfifteen theme directory for now,
  // until we figure how to share themes/plugins?
  $exists = provision_file()->exists($wp_content_dir . '/themes/twentyfifteen')
    ->succeed('WordPress: @path exists.')
    ->status();

  if (! $exists) {
    provision_file()->symlink(d()->root . '/wp-content/themes/twentyfifteen', $wp_content_dir . '/themes/twentyfifteen')
      ->succeed('Created symlink @path to @target')
      ->fail('Could not create symlink @path to @target: @reason');
  }
}

/**
 * Helper function to call wp-cli commands.
 */
function provision_wordpress_wpcli_run(String $args) {
  $root = d()->root . '/';
  $drushrc = d()->root . '/sites/' . d()->uri . '/drushrc.php';

  drush_shell_exec_interactive("/var/aegir/.composer/vendor/bin/wp --path=$root --require=$drushrc $args");
}
