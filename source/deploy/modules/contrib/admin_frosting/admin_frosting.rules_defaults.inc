<?php
/**
 * @file
 * admin_frosting.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function admin_frosting_default_rules_configuration() {
  $items = array();
  $items['rules_admin_frosting_close_comments_on_content'] = entity_import('rules_config', '{ "rules_admin_frosting_close_comments_on_content" : {
      "LABEL" : "Admin Frosting close comments on content",
      "PLUGIN" : "action set",
      "TAGS" : [ "Admin Frosting" ],
      "REQUIRES" : [ "rules" ],
      "USES VARIABLES" : { "node" : { "label" : "Node", "type" : "node" } },
      "ACTION SET" : [ { "data_set" : { "data" : [ "node:comment" ], "value" : "1" } } ]
    }
  }');
  $items['rules_admin_frosting_hide_comments_on_content'] = entity_import('rules_config', '{ "rules_admin_frosting_hide_comments_on_content" : {
      "LABEL" : "Admin Frosting hide comments on content",
      "PLUGIN" : "action set",
      "TAGS" : [ "Admin Frosting" ],
      "REQUIRES" : [ "rules" ],
      "USES VARIABLES" : { "node" : { "label" : "Node", "type" : "node" } },
      "ACTION SET" : [ { "data_set" : { "data" : [ "node:comment" ], "value" : "0" } } ]
    }
  }');
  $items['rules_admin_frosting_open_comments_on_content'] = entity_import('rules_config', '{ "rules_admin_frosting_open_comments_on_content" : {
      "LABEL" : "Admin Frosting open comments on content",
      "PLUGIN" : "action set",
      "TAGS" : [ "Admin Frosting" ],
      "REQUIRES" : [ "rules" ],
      "USES VARIABLES" : { "node" : { "label" : "Node", "type" : "node" } },
      "ACTION SET" : [ { "data_set" : { "data" : [ "node:comment" ], "value" : "2" } } ]
    }
  }');
  return $items;
}
