<?php

/**
 * Implements hook_provision_verify().
 *
 * NB: for wpsite, we implement instead a wordpress-verify task,
 * because verify calls things that do too many Drupal assumptions.
 * We should probably also call it for the wpplatform.
 */
function drush_provision_wordpress_provision_verify($url = NULL) {
  if (d()->type == 'wpplatform') {
    drush_log(dt("WordPress: Running drush_wordpress_provision_verify on !type, name !name", array('!type' => d()->type, '!name' => d()->name)));

    provision_file()->exists(d()->root . '/wp-includes')
      ->succeed('Directory @path seems to be a WordPress installation.')
      ->fail('Directory @path does not seem to be a valid Wordpress installation (missing wp-includes).', 'PROVISION_WORDPRESS_INVALID_ROOT');

    d()->service_subscribe('server', '@server_master');
    d()->service_subscribe('http', '@server_master');
    d()->service_subscribe('wpplatform', d()->name);

    // Writes the wp-config.php file in the platform root.
    $config = new Provision_Config_wpplatform(d()->name);
    $config->write();

    # FIXME: needs more testing
    # d()->service('http')->write();
  }
  elseif (d()->type == 'wpsite') {
    drush_log(dt("WordPress: Running drush_wordpress_provision_verify on !type, name !name", array('!type' => d()->type, '!name' => d()->name)));

    d()->service_subscribe('server', '@server_master');
    d()->service_subscribe('http', '@server_master');
    d()->service_subscribe('wpplatform', d()->name);

    # d()->service('http')->create_config('site');
    # d()->service('http')->write();
  }
}

/**
 * Inject the relevant Apache configuration in the site vhost.
 * Implements drush_hook_provision_apache_vhost_config().
 */
function provision_wordpress_provision_apache_vhost_config($uri, $data) {
  if (d()->type != 'wpsite') {
    return "";
  }

  drush_log('WordPress: hello from provision_wordpress_provision_apache_vhost_config');

  $site_path = d()->site_path;
  $site_name = d()->uri;
  $platform_root = d()->root;

  $conf = <<<EOF

    RewriteEngine On

    SetEnv wp_content_dir {$site_path}/wp-content/
    RewriteRule ^/wp-content/(.*)$ /sites/{$site_name}/wp-content/$1 [QSA,L]

    <Directory $platform_root>
      RewriteBase /
      RewriteRule ^index\.php$ - [L]
      RewriteCond %{REQUEST_FILENAME} !-f
      RewriteCond %{REQUEST_FILENAME} !-d
      RewriteRule . /index.php [L]

      Order allow,deny
      Allow from all
      Satisfy any
      Require all granted

      # Do not read any .htaccess in the platform
      AllowOverride none
    </Directory>

EOF;

  drush_log('WordPress: leaving provision_wordpress_provision_apache_vhost_config');
  return $conf;
}

/**
 * Append Nginx configuration to site vhost configuration.
 * Implements drush_hook_provision_nginx_server_config().
 *
 * FIXME: this is only to test the installation.
 * We will want to block install.php, but might want to allow other scripts.
 */
function provision_wordpress_provision_nginx_vhost_config($uri, $data) {
  if (d()->type != 'wpsite') {
    return "";
  }

  drush_log('WordPress: hello from provision_wordpress_provision_nginx_vhost_config', 'ok');

  // FIXME: Quick & dirty confs. There is probably a better way.
  $site_path = d()->site_path;
  $phpfpm_socket_path = Provision_Service_http_nginx::getPhpFpmSocketPath();

  $conf = <<<EOF
    fastcgi_param wp_content_dir {$site_path}/wp-content/;

    ###
    ### Allow some known php files.
    ###
    location ~ /wp-admin/\$ {
      tcp_nopush   off;
      keepalive_requests 0;
      access_log /var/log/nginx/access.log main;
      if (\$is_bot) {
        return 403;
      }
      try_files    /wp-admin/index.php =404;
      fastcgi_pass unix:<?php print $phpfpm_socket_path; ?>;
    }
    location ~ /wp-admin/(.*).php\$ {
      tcp_nopush   off;
      keepalive_requests 0;
      access_log /var/log/nginx/access.log main;
      if (\$is_bot) {
        return 403;
      }
      try_files    \$uri =404;
      fastcgi_pass unix:<?php print $phpfpm_socket_path; ?>;
    }
    location ~ /wp-(.*).php\$ {
      tcp_nopush   off;
      keepalive_requests 0;
      access_log /var/log/nginx/access.log main;
      if (\$is_bot) {
        return 403;
      }
      try_files    \$uri =404;
      fastcgi_pass unix:<?php print $phpfpm_socket_path; ?>;
    }
    location ~ ^/wp-content/uploads/(.*)\$ {
      try_files /sites/\$main_site_name/wp-content/uploads/\$1 =404;
    }

EOF;

  $server = d()->wpplatform->web_server;
  if (($server->Certificate_service_type == 'LetsEncrypt') && ($challenge_path = $server->letsencrypt_challenge_path)) {
    drush_log(dt("Injecting Let's Encrypt 'well-known' ACME challenge directory ':path' into Nginx vhost entry.", array(
      ':path' => $challenge_path,
    )));

    $lines = array();
    $lines[] = "  # Allow access to letsencrypt.org ACME challenges directory.";
    $lines[] = "  location ^~ /.well-known/acme-challenge {";
    $lines[] = "    alias {$challenge_path};";
    $lines[] = "    try_files \$uri 404;";
    $lines[] = "  }";
    $lines[] = "\n";
    return implode("\n", $lines);
  }

  drush_log('WordPress: leaving provision_wordpress_provision_nginx_vhost_config', 'ok');
  return $conf;
}
