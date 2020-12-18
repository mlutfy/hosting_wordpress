<?php

/**
 * Directory/file checks for install and verify.
 */
function provision_wordpress_setup_directories() {
  $wp_content_dir = d()->site_path . '/wp-content';

  $content_dirs = [
    d()->site_path,
    $wp_content_dir,
    $wp_content_dir . '/languages',
    $wp_content_dir . '/plugins',
    $wp_content_dir . '/themes',
    $wp_content_dir . '/uploads',
  ];

  foreach ($content_dirs as $dir) {
    $exists = provision_file()->exists($dir)
      ->succeed('WordPress: @path exists.')
      ->status();

    if (!$exists) {
      $exists = provision_file()->mkdir($dir)
        ->succeed('Wordpress: @path has been created.')
        ->fail('CiviCRM: @path could not be created.', 'DRUSH_PERM_ERROR')
        ->status();
    }

    // setgid directories, it avoids a few permissions issues
    // where files are aegir.www-data.
    provision_file()->chmod($dir, 02750, TRUE)
      ->succeed('Changed permissions of @path to @perm')
      ->fail('Could not change permissions <code>@path to @perm')
      ->status();

    provision_file()->chgrp($dir, d('@server_master')->web_group, TRUE)
      ->succeed('Changed group ownership @path to @gid')
      ->fail('Could not change group ownership of @path to @gid')
      ->status();
  }

  // Directories writable by www-data
  $writable_dirs = [
    $wp_content_dir . '/languages',
    $wp_content_dir . '/uploads',
  ];

  foreach ($writable_dirs as $dir) {
    provision_file()->chmod($dir, 02770, TRUE)
      ->succeed('Changed permissions of @path to @perm')
      ->fail('Could not change permissions <code>@path to @perm')
      ->status();
  }

  // Symlink themes and plugins from the platform to the site
  provision_wordpress_symlink_subdirs('themes');
  provision_wordpress_symlink_subdirs('plugins');
}

/**
 * Symlink all plugins or themes from the platform, if not locally overriden.
 */
function provision_wordpress_symlink_subdirs($subdir) {
  $dir = new DirectoryIterator(d()->root . '/wp-content/' . $subdir);

  foreach ($dir as $fileinfo) {
    if (!$fileinfo->isDot()) {
      $fname = $fileinfo->getFilename();
      $wp_content_dir = d()->site_path . '/wp-content';

      if (!file_exists("$wp_content_dir/$subdir/$fname")) {
        provision_file()->symlink(d()->root . "/wp-content/$subdir/$fname", "$wp_content_dir/$subdir/$fname")
          ->succeed('Created symlink @path to @target')
          ->fail('Could not create symlink @path to @target: @reason');
      }
    }
  }
}

/**
 * Helper function to call wp-cli commands.
 */
function provision_wordpress_wpcli_run(String $args) {
  $root = d()->root . '/';
  $drushrc = d()->root . '/sites/' . d()->uri . '/drushrc.php';

  drush_shell_exec_interactive("wp --path=$root --require=$drushrc $args");
}

/**
 * Rewrites the domains.txt for dehydrated (https/letsencrypt).
 */
function provision_wordpress_update_domainstxt() {
  $lines = [];

  // Fetch all sites and their aliases
  // Aegir normally uses 'services' to communicate between the frontend and backend
  // (because these drush functions are being run in the context of the site, not aegir),
  // Instead of doing that, we are using the Ansible inventory, easier in a way, especially
  // we want to slowly move further torwards Ansible.

  // Get the Aegir frontend URL (@todo move to a function)
  // @todo This makes many assumptions
  global $aliases;
  require_once '/var/aegir/.drush/hostmaster.alias.drushrc.php';
  $frontend = 'https://' . $aliases['hostmaster']['uri'] . '/inventory';

  $data = file_get_contents($frontend);
  $data = json_decode($data, true);

  if (!empty($data['wordpress_sites']['hosts'])) {
    foreach ($data['wordpress_sites']['hosts'] as $uri) {
      $line = $uri;

      if (!empty($data['_meta']['hostvars'][$uri]['aliases'])) {
        foreach ($data['_meta']['hostvars'][$uri]['aliases'] as $alias) {
          $line .= ' ' . $alias;
        }
      }

      $lines[] = $line;
    }
  }

  $fp = fopen('/var/aegir/config/letsencrypt/domains-wp.txt', 'w');
  fwrite($fp, implode("\n", $lines) . "\n");
  fclose($fp);

  // Check if the config-wp file needs to be written
  if (!file_exists('/var/aegir/config/letsencrypt/config-wp')) {
    $fp = fopen('/var/aegir/config/letsencrypt/config-wp', 'w');
    fwrite($fp, 'WELLKNOWN="/var/aegir/config/letsencrypt.d/well-known/acme-challenge"' . "\n"
      . 'DOMAINS_TXT="/var/aegir/config/letsencrypt/domains-wp.txt"' . "\n"
      . 'PARAM_CERTDIR="/var/aegir/config/letsencrypt.d/"' . "\n");
    fclose($fp);
  }
}

/**
 * Rewrites the wp-cli.yml file in the site root.
 */
function provision_wordpress_update_wpcliyml() {
  $fp = fopen(d()->site_path . '/wp-cli.yml', 'w');

  // @todo Check if https is enabled, i.e. if a cert is present?
  fwrite($fp, "path: " . d()->root . "\n"
    . "url: https://" . d()->uri . "\n");

  fclose($fp);
}
