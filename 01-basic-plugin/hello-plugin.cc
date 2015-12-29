// BASIC PLUGIN 01
//
// This plugin demostrates how a specific function is invoked in a plugin.
// Here hello_plugin_start() is invoked where one can insert one's code 
// to write a plugin.
//
// Since this demo plugin is inserted after 'cfg' pass in the 'tree' phase 
// of compilation the user function is invoked for each function. In the 
// 'ipa' phase the function is invoked only once.
//
//
#include <iostream>

//This is the first gcc header to be included
#include "gcc-plugin.h"
#include "plugin-version.h"

#include "tree-pass.h"
#include "context.h" // gives the variable 'g'

// We must assert that this plugin is GPL compatible
int plugin_is_GPL_compatible;

void hello_plugin_start(function *fun);

static struct plugin_info my_gcc_plugin_info = { 
    "1.0", 
    "A Hello Plugin to invoke a user function." };

const pass_data hello_plugin_data  = 
{
    GIMPLE_PASS,
    "hello-plugin",            /* name */
    OPTGROUP_NONE,             /* optinfo_flags */
    TV_NONE,                   /* tv_id */
    PROP_gimple_any,           /* properties_required */
    0,                         /* properties_provided */
    0,                         /* properties_destroyed */
    0,                         /* todo_flags_start */
    0                          /* todo_flags_finish */
};

// using class hello_plugin here leads to error
struct hello_plugin : gimple_opt_pass {
    public:
        hello_plugin(gcc::context *ctx)
            : gimple_opt_pass(hello_plugin_data, ctx) {
        }

        virtual unsigned int execute(function *fun) override {
            hello_plugin_start(fun);

            return 0;
        }
};

/**
 * This function is called for each function in the source program begin 
 * compiled. This function is called only once int the 'ipa' phase.
 */
void hello_plugin_start(function *fun) {
    std::cerr << "\n" << "Hello Plugin STARTED" << "\n";

    // INSERT CODE HERE ##############################################

    std::cerr << "\n" << "Hello Plugin ENDED" << "\n";
}

int plugin_init (struct plugin_name_args *plugin_na,
        struct plugin_gcc_version *version) {
    // We check the current gcc loading this plugin against the gcc we used to
    // created this plugin
    if (!plugin_default_version_check (version, &gcc_version)) {
        std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR
            << "." << GCCPLUGIN_VERSION_MINOR << "\n";
        return 1;
    }

    std::cerr << "Hello Plugin initializing" << std::endl;

    register_callback(
            plugin_na->base_name,
            PLUGIN_INFO,/*event*/ 
            NULL,/*callback*/ 
            &my_gcc_plugin_info);/*userdata*/ 

    // Register the phase right after cfg
    struct register_pass_info pass_info;

    pass_info.pass = new hello_plugin(g);
    pass_info.reference_pass_name = "cfg";
    pass_info.ref_pass_instance_number = 1;
    pass_info.pos_op = PASS_POS_INSERT_AFTER;

    register_callback ( 
            plugin_na->base_name,
            PLUGIN_PASS_MANAGER_SETUP,
            NULL,
            &pass_info);

    std::cerr << "Hello Plugin initialized" << std::endl;

    return 0;
}
