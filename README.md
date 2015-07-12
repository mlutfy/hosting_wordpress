Hosting WordPress
=================

EXPERIMENTAL work in progress. Use at your own risk.

This module is not officially supported. No security announcement will be
published if an issue is found. See 'support' for more information.

See: https://www.drupal.org/node/1044692

Installation
============

*IMPORTANT:* run all the following commands as the 'aegir' user ('sudo -i -u aegir' or 'su -c /bin/bash aegir').

Copy the hosting_wordpress module in your ~/hostmaster-7.x-3.x/sites/example.org/modules directory:

    cd ~/hostmaster-7.x-3.x/sites/example.org/modules/
    git clone https://github.com/mlutfy/hosting_wordpress.git

Enable the hosting_wpplatform and hosting_wpsite modules:

    drush @hostmaster en hosting_wpsite

Run a 'verify' task on the Hostmaster so that Aegir does the magic include in ~/.drush/drushrc.php (c.f. do#2300537):

    drush @hostmaster provision-verify

Install wp-cli using composer (this should create a sylink in ~/.composer/vendor/bin/wp)

    cd ~/
    composer global require wp-cli/wp-cli

Make sure your Aegir user has a ~/.profile file, so that the .bashrc is read when using 'sudo -i -u aegir':

    cp -i /etc/skel/.profile ~/

Composer adds its 'bin' directory to the $PATH, which makes it possible to use the 'wp' command easily.

Finally, apply the 2 patches included in the patches directory (this may break install/verify of Drupal sites)

NB: when applying the patches, 'provision' is usually located in /usr/share/drush/commands/, and 'hosting' is in ~/hostmaster-7.x-3.0-beta1/profiles/hostmaster/modules/aegir/hosting/.

How to test
===========

- Create a new platform on your filesystem, ex: mkdir /var/aegir/platforms/wordpress-4.2.2/; cd /var/aegir/platforms/wordpress-4.2.2/; wp core download
- Create a new WP Platform (node/add/wpplatform)
- Create a new WP Site from there.
- Access the URL of your new site.
- NB: I have only tested on nginx so far. There may be rewrites missing for Apache.
- You can also use wp-cli in a site context, using, for example: "drush @mysite.example.org wp user list"

TODO and known bugs
===================

* Define WP_CONTENT_DIR in wp-config.php to separate content.
* Can't run 'verify' on the platform (workaround: edit the platform node and re-save).
* CiviCRM support? https://github.com/andy-walker/wp-cli-civicrm

How it works
============

- provision_wordpress and hosting_wordpress implement new entities for
  the 'wpplatform' and 'wpsite' (highly inspired from 'platform' and 'site').

- while it would be great to re-use the platform/site from Aegir, they make
  too many assumptions related to Drupal.

- wpplatform verify creates a platform/example/wp-config.php file
  based on wp-config-sample.php, but uses the vhost variables so that
  we can install multiple sites in the same platform.

Support
=======

This module is not currently supported. Feel free to send patches using the issue queue on github.

Want to help make this happen? Participate to our crowd-funding campaign:  
https://crm.symbiotic.coop/civicrm/contribute/transact?id=2

Commercial support is available through Coop SymbioTIC:  
https://www.symbiotic.coop

Or contact one of the Aegir service providers:  
https://www.drupal.org/project/hosting

Copyright and license
=====================

Provision Wordpress  
(C) 2015 Mathieu Lutfy <mathieu@symbiotic.coop>

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
