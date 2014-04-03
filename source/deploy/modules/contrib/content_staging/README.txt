
INTRODUCTION
============

This module allows the content for a site to be kept in a separate staging
repository. This will typically be a Subversion or Git repository where the
content has been committed and is being maintained.

The module checks out the content directly into the nodes of the site from
whatever branch of the repository it is associated with. It is possible to have
multiple sites associated with different branches so that future or
experimental versions of content can be previewed and tested before being
committed to live.

 * The text can be stored in the repository in html
 * However the preferred method is to use the Writeup format (a kind of
   enhanced Markdown).
    o See: writeup.org/quickref ( writeup.org/quickref )
 * Variables within the text file can control the process of updating the site
   from staging
 * A future enhancement will allow updating to take place in a cron job

DIFFERENCES FROM OTHER CONTENT STAGING MODULES
==============================================

 _____________________________________________________________________
|Module |Method                             |Status                   |
|_______|___________________________________|_________________________|
|publish|Push and pull models, using XML-RPC|Seems to be inactive     |
|_______|___________________________________|_________________________|
|deploy |Sophisticated model using services,|Under active development.|
|       |sponsored by Al Jazeera            |                         |
|_______|___________________________________|_________________________|
The Content Staging module is much simpler than the above and is intended to be
used in an environment where much of the content is not created directly online
(using Drupal as an editing tool). Instead it is created offline, using a text
editor or some other tool and then committed to the content repository. The
Content Staging module then checks out the appropriate branch into the Drupal
website(s), running the text through the Writeup pre-processor as it does this.

Although this may be initially more work to set up, the result is robust and
performant, and gives a fine-grained level of version control over the content,
especially in a situation of multiple authorship. In short, all the
sophistication that we expect for managing the version control of code is now
available for content.


DISCUSSIONS WITHIN THE DRUPAL COMMUNITY
---------------------------------------
The following discussions are taking place:

 * Drupalcon London: Content Staging and Deployments in Drupal 7
   ( london2011.drupal.org/conference/sessions/content-staging-and-deployments-
   drupal-7 )
 * Large Scale Drupal (LSD) - Projects and Plans
   ( groups.drupal.org/large-scale-drupal-lsd-projects-and-plans )
 * Content Staging Initiative (CSI)
   ( groups.drupal.org/large-scale-drupal-lsd-projects-and-plans/content-
   staging )
 * Staging Content to Production Servers
   ( groups.drupal.org/taxonomy/term/64088 )
 * Denver BOF on Content Staging in Drupal 7
   ( denver2012.drupal.org/bof/content-staging-drupal-7 )

CONFIGURATION
=============

1. Install the module
2. Download the Writeup binary from one of:
    o https://writeup.googlecode.com/svn/bin/writeup
    o http://writeup.sourceforge.net
   And copy it to your server, e.g. into the directory /opt/writeup

3. Set up your repository to check out the content into the content staging
   area, e.g.
    o svn co https://mycontent.googlecode.com/svn/trunk /var/www/d7/staging
    o make sure the web server has write access to the staging directory if you
      want it to run updates
4. Go to admin/Content Staging and set
    o the path to the Writeup binary, e.g. /opt/writeup
    o the root directory of your content staging area, e.g. staging
    o the update script for updating from the staging repo, e.g. svn up
5. Visit the status page: /cs_status to view the status of your content and to
   update the site
6. Note that nodes in staging are mapped to nodes on the site by the page alias
   == path/filename
    o e.g. the file about/me.txt will map to the node with the alias about/me
    o At least for now, all nodes have to be created manually initially (with
      no content or title needed) for the content to be pushed to them. It is
      hoped that this limitation will be addressed in a future release.

DOCUMENTATION
=============

 * More documentation to follow.

TO DO
=====

 * Create nodes automatically on the server when new nodes appear in staging
 * Add drush support

SUPPORT
=======

If you experience a problem with Content Staging or have a problem, file a
request or issue on the Content Staging queue at drupal.org/project/issues/
content_staging. DO NOT POST IN THE FORUMS. Posting in the issue queues is a
direct line of communication with the module authors.

No guarantee is provided with this software, no matter how critical your
information, module authors are not responsible for damage caused by this
software or obligated in any way to correct problems you may experience.


SPONSORS
========

The Content Staging module is sponsored by Love in Truth
( loveintruth.com ).

Licensed under the GPL 2.0.
www.gnu.org/licenses/gpl-2.0.txt

