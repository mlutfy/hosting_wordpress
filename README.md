Hosting WordPress
=================

EXPERIMENTAL work in progress. Use at your own risk.

This module is not officially supported. No security announcement will be
published if an issue is found. See 'support' for more information.

See: https://www.drupal.org/node/1044692

Requirements
============

* Aegir 3 (7.x-3.x)
* Web server: nginx (Apache might work, but not well tested)

The [provision_symbiotic](https://github.com/coopsymbiotic/provision_symbiotic) module is recommended.
It provides a few workarounds for the nginx vhost template.

The [aegir_ansible](https://www.drupal.org/project/aegir_ansible) module is recommended if using https.
This module will automatically write a domains-wp.txt file to regenerate https certificates with letsencrypt.
Currently a cron is not automatically setup. See the https section below.

Installation
============

*IMPORTANT:* run all the following commands as the 'aegir' user ('sudo -i -u aegir' or 'su -c /bin/bash aegir').

Copy the `hosting_wordpress` module in your ~/hostmaster-7.x-3.x/sites/example.org/modules directory:

    cd ~/hostmaster-7.x-3.x/sites/example.org/modules/
    git clone https://github.com/mlutfy/hosting_wordpress.git

Enable the `hosting_wpplatform` and `hosting_wpsite` modules:

    drush @hostmaster en hosting_wpsite

Run a 'verify' task on the Hostmaster so that Aegir does the magic include in `~/.drush/drushrc.php` (c.f. do#2300537):

    drush @hostmaster provision-verify

Make sure your Aegir user has a `~/.profile` file, so that the .bashrc is read when using `sudo -i -u aegir`:

    cp -i /etc/skel/.profile ~/

Composer adds its 'bin' directory to the $PATH, which makes it possible to use the 'wp' command easily.

Install `wp-cli` using composer (this should create a sylink in `~/.composer/vendor/bin/wp`)

    cd ~/
    composer global require wp-cli/wp-cli

Alternatively, if `wp` is not found, add the composer path manually to your `.bashrc`:

    echo 'export PATH="$PATH:$HOME/.composer/vendor/bin"' >> ~/.bashrc

Warning
=======

Because of limitations in the integration with `provision_customhtaccess` (which lets developers inject
custom rules in their nginx configuration), this module assumes that you trust the developers who have
access to the site-specific directory of their site.

If a file called `sites/example.org/nginx-custom.conf` exists, it will be
included by the nginx configuration.

An incorrect nginx configuration can cause problems with restarting nginx, potentially provide a path
towards privilege escalation, etc.

How to test
===========

- Create a new platform on your filesystem, ex: mkdir /var/aegir/platforms/wordpress-4.5/; cd /var/aegir/platforms/wordpress-4.5/; wp core download
- Create a new WP Platform (node/add/wpplatform)
- Create a new WP Site from there.
- Access the URL of your new site.
- You can also use wp-cli in a site context, using, for example: "drush @mysite.example.org wp user list"

Known issues
============

See: https://github.com/mlutfy/hosting_wordpress/issues

How it works
============

- provision_wordpress and hosting_wordpress implement new entities for
  the 'wpplatform' and 'wpsite' (highly inspired from 'platform' and 'site').

- while it would be great to re-use the platform/site from Aegir, they make
  too many assumptions related to Drupal.

- wpplatform verify creates a platform/example/wp-config.php file
  based on wp-config-sample.php, but uses the vhost variables so that
  we can install multiple sites in the same platform.

Https
=====

This module works a bit differently from `hosting_https`. It still assumes that the
module has been enabled and is used on the main Aegir frontend, but works more closely
to how [dehydrated](https://github.com/dehydrated-io/dehydrated/) works.

As an admin, to enable https on a site:

* Make sure that "hosting https" is enabled on the Aegir frontend (i.e. you can access your
  aegir instance with https, ex: https://aegir.example.org).
* Create a new WordPress site.
* Run 'Verify' on the new site (eventually this will run automatically).

Technically:

* When sites are verified, it checks if an https certificate is already available.
  If so, it assumes that the vhost supports and redirects to https.
* If the 'aegir ansible' module is installed, it fetches a list of sites (and aliases)
  from the inventory and writes a 'domains-wp.txt' file in `/var/aegir/config/letsencrypt/`.
* If 'hosting_https' seems to be enabled (well, if the above directory exists), it will
  assume that sites must be https, and will generate a certificate for it.

You can then configure a cron to run daily:

```
/var/aegir/config/letsencrypt/script -c -f /var/aegir/config/letsencrypt/config-wp
```

You can also run the above to debug if a certificate is not renewing correcly.

The config-wp file should be generated if empty. If not, you can create it manually:

```
WELLKNOWN="/var/aegir/config/letsencrypt.d/well-known/acme-challenge"
DOMAINS_TXT="/var/aegir/config/letsencrypt/domains-wp.txt"
PARAM_CERTDIR="/var/aegir/config/letsencrypt.d/"
```

This module will also attempt to run dehydrated when a new site is created, using the above command.

CiviCRM
=======

See: https://drupal.org/project/hosting_civicrm

Support
=======

Feel free to send patches using the issue queue on Github.

Commercial support is available through Coop SymbioTIC:  
https://www.symbiotic.coop

Copyright and license
=====================

Provision Wordpress  
(C) 2015-2020 Mathieu Lutfy <mathieu@symbiotic.coop>

Distributed under the terms of the GNU Affero General public license v3 (AGPL).  
http://www.gnu.org/licenses/agpl.html

```
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
