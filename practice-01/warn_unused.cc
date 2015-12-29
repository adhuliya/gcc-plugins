#include <iostream>
#include <cassert>
#include <set>
#include <utility>
#include <algorithm>

// This is the first gcc header to be included
#include "gcc-plugin.h"
#include "plugin-version.h"

#include "cp/cp-tree.h"
#include "context.h"
#include "function.h"
#include "internal-fn.h"
#include "is-a.h"
#include "predict.h"
#include "basic-block.h"
#include "tree.h"
#include "tree-ssa-alias.h"
#include "gimple-expr.h"
#include "gimple.h"
#include "gimple-ssa.h"
#include "tree-pretty-print.h"
#include "tree-pass.h"
#include "tree-ssa-operands.h"
#include "tree-phinodes.h"
#include "gimple-pretty-print.h"
#include "gimple-iterator.h"
#include "gimple-walk.h"
#include "diagnostic.h"
#include "stringpool.h"
#include "print-tree.h"

#include "ssa-iterators.h"

// We must assert that this plugin is GPL compatible
int plugin_is_GPL_compatible;

static struct plugin_info my_gcc_plugin_info = { "1.0", "This plugin is under development." };

namespace
{
    const pass_data warn_unused_result_cxx_data = 
    {
        GIMPLE_PASS,
        "practice-01", /* name */
        OPTGROUP_NONE,             /* optinfo_flags */
        TV_NONE,                   /* tv_id */
        PROP_gimple_any,           /* properties_required */
        0,                         /* properties_provided */
        0,                         /* properties_destroyed */
        0,                         /* todo_flags_start */
        0                          /* todo_flags_finish */
    };

    struct warn_unused_result_cxx : gimple_opt_pass
    {
        warn_unused_result_cxx(gcc::context *ctx)
            : gimple_opt_pass(warn_unused_result_cxx_data, ctx)
        {
        }

        virtual unsigned int execute(function *fun) override
        {
            // This phase has two steps, first we remove redundant LHS from GIMPLE_CALLs
            std::set<tree> unused_lhs = gather_unused_lhs(fun);
            warn_unused_result_lhs(unused_lhs, fun);

            process_function(fun);

            return 0;
        }

        virtual warn_unused_result_cxx* clone() override
        {
            // We do not clone ourselves
            return this;
        }
    }

    int plugin_init (struct plugin_name_args *plugin_info,
            struct plugin_gcc_version *version)
    {
        // We check the current gcc loading this plugin against the gcc we used to
        // created this plugin
        if (!plugin_default_version_check (version, &gcc_version))
        {
            std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR << "." << GCCPLUGIN_VERSION_MINOR << "\n";
            return 1;
        }

        register_callback(plugin_info->base_name,
                /* event */ PLUGIN_INFO,
                /* callback */ NULL, /* user_data */ &my_gcc_plugin_info);

        // Register the phase right after cfg
        struct register_pass_info pass_info;

        pass_info.pass = new warn_unused_result_cxx(g);
        pass_info.reference_pass_name = "cfg";
        pass_info.ref_pass_instance_number = 1;
        pass_info.pos_op = PASS_POS_INSERT_AFTER;

        register_callback (plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);

        return 0;
    }
}
