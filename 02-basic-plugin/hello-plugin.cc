/* BASIC PLUGIN 04
This plugin runs in IPA phase after 'pta' pass.

It prints the functions and their basic blocks with their gimple assignement statements
and the detailed print_node() information of its operands (lhs and rhs) to the standard
output.

To build:
$ make
To invoke:
$ gcc -fplugin=./hello-plugin.so -c test.c
OR
$ make test
 */
#include <iostream>
#include <stdio.h>

//This is the first gcc header to be included
#include "gcc-plugin.h"
#include "plugin-version.h"

#include "tree-pass.h"
#include "context.h" // gives the variable 'g'

#include "tree.h" 
#include "cgraph.h"     // tree.h ===> cgraph.h

#include "gimple-expr.h"

#include "basic-block.h"
#include "tree-ssa-alias.h"
#include <gimple.h> // basic-block.h, tree-ssa-alias.h, gimple-expr.h ===> gimple.h 
#include "gimple-iterator.h" // gimple.h ===> gimple-iterator.h

#include "gimple-pretty-print.h"

#include "print-tree.h"

// We must assert that this plugin is GPL compatible
int plugin_is_GPL_compatible;

void hello_plugin_start(function *fun);
void print_gimple_assign_stmt_operands(gimple stmt);

static struct plugin_info my_gcc_plugin_info = { 
    "1.0", 
    "This plugin prints the gimple stmt and its tree." };

const pass_data hello_plugin_data  = 
{
    SIMPLE_IPA_PASS,
    "hello-plugin",            /* name */
    OPTGROUP_NONE,             /* optinfo_flags */
    TV_NONE,                   /* tv_id */
    0,                         /* properties_required */
    0,                         /* properties_provided */
    0,                         /* properties_destroyed */
    0,                         /* todo_flags_start */
    0                          /* todo_flags_finish */
};

// using class hello_plugin here leads to error
struct hello_plugin : simple_ipa_opt_pass {
    public:
        hello_plugin(gcc::context *ctx)
            : simple_ipa_opt_pass (hello_plugin_data, ctx) {
        }

        virtual unsigned int execute(function *fun) override {
            hello_plugin_start(fun);

            return 0;
        }
};

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
    pass_info.reference_pass_name = "pta";
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

// in a SIMPLE_IPA_PASS this function is called only once
// in a GIMPLE_PASS this function is called for each function
void hello_plugin_start(function *fun) {
    std::cerr << "\n" << "Hello Plugin STARTED" << "\n";

    struct cgraph_node *node;

    // Printing functions with check
    std::cerr << "\n" << "Hello Plugin FILTERED FUNCTIONS START" << "\n";
    FOR_EACH_FUNCTION (node) {
        // filtering functions
        if (!gimple_has_body_p(node->decl) ||  node->clone_of) {
            continue;
        }

        push_cfun (DECL_STRUCT_FUNCTION (node->decl));

        std::cerr << "\n" << "Function : " << node->name() << std::endl;

        basic_block bb;
        FOR_EACH_BB_FN (bb, cfun) {
            std::cerr << "\n########\n" << "BB : " << bb->index << "\n########\n";

            gimple_stmt_iterator gsi;

            for (gsi = gsi_start_bb (bb); !gsi_end_p (gsi); gsi_next (&gsi)) {
                gimple stmt = gsi_stmt (gsi);

                // print_gimple_stmt (stdout, stmt, 0, 0);

                if (gimple_code (stmt) == GIMPLE_ASSIGN) {
                    std::cerr << "\n--------------------------------\n";
                    std::cerr << std::endl << "\t"; 
                    
                    print_gimple_stmt (stdout, stmt, 0, 0);

                    std::cerr << std::endl << "\t"; 

                    print_gimple_assign_stmt_operands(stmt);

                    std::cerr << "\n--------------------------------\n";
                }
            }
        }

        // restoring the context by popping the cfun
        pop_cfun ();
    }
    std::cerr << "\n" << "Hello Plugin FILTERED FUNCTIONS END" << "\n";

    std::cerr << "\n" << "Hello Plugin ENDED" << "\n";
}

void print_gimple_assign_stmt_operands(gimple stmt) {
    tree lhs_op = NULL, op1 = NULL, op2 = NULL, op3 = NULL;

    /* Extract operands of the assignment statement
     * API's can be found in the file gimple.h */
    switch (gimple_num_ops (stmt)) {
        case 4:
            op3 = gimple_assign_rhs3 (stmt);
            print_node (stdout, "\nRHS3: ", op3, 0);
        case 3:
            op2 = gimple_assign_rhs2 (stmt);
            print_node (stdout, "\nRHS2: ", op2, 0);
        case 2:
            op1 = gimple_assign_rhs1 (stmt);
            print_node (stdout, "\nRHS1: ", op1, 0);

            lhs_op = gimple_assign_lhs (stmt);
            print_node (stdout, "\nLHS : ", lhs_op, 0);
            break;
        default:
            gcc_unreachable();
    }
}
