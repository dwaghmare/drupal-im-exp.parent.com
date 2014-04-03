ctex is a template for making a configuration-based module which uses ctools to
handle import/export as well as the CRUD UI. It is designed to jump-start the
process of creating these types of modules.

You should not install ctex, but overwrite it to make your own module.

INSTRUCTIONS:

1) Choose a unique name for your module.
For our example, we'll be working with presets for reporting settings, so the
module will be called "reports".

2) Using your favourite IDE or text editor, do a global replace on:
  "ctex" => "reports"
  "Ctex" => "Reports"

3) Rename the four files which contain ctex in the title...
  ctex.info => reports.info
  ctex.install => reports.install
  ctex.module => reports.module
  plugins/export_ui/ctex_ctools_export_ui.inc =>
    plugins/export_ui/reports_ctools_export_ui.inc

4) ctex uses the default exportable name of "preset". If you would prefer
something else, globally replace "preset" with your desired exportable name.

5) In the .info file, review and change the Name and Description.

6) In the export_ui plugin, update the plugin settings to match desired menu and
singular/plural names.

7) In the .install file, update the schema to match your configuration. A couple
of fields have been added to get your started. Be aware that ctools will inject
a "type" key into your object, so don't use one in the schema.

8) Update HOOK_ctools_export_ui_form() in the export_ui file to match your
desired configuration. Field names in the edit form should MATCH your schema.

9) Update HOOK_default_MYMODULENAME_preset() with your new configuration.