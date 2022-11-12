//! ----------------------------------------------------------------------------
//! Copyright Edgio Inc.
//!
//! \file:    TODO
//! \details: TODO
//!
//! Licensed under the terms of the Apache 2.0 open source license.
//! Please refer to the LICENSE file in the project root for the terms.
//! ----------------------------------------------------------------------------
#include <is2/srvr/srvr.h>
#include <is2/srvr/lsnr.h>
#include <is2/handler/file_h.h>
#include <is2/support/trace.h>
#include <is2/support/ndebug.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#define ENABLE_PROFILER 1
#ifdef ENABLE_PROFILER
#include <gperftools/profiler.h>
#include <gperftools/heap-profiler.h>
#endif
//! ----------------------------------------------------------------------------
//! const
//! ----------------------------------------------------------------------------
#ifndef STATUS_OK
#define STATUS_OK 0
#endif
#ifndef STATUS_ERROR
#define STATUS_ERROR 0
#endif
//! ----------------------------------------------------------------------------
//! globals
//! ----------------------------------------------------------------------------
ns_is2::srvr *g_srvr = NULL;
//! ----------------------------------------------------------------------------
//! \details: sigint signal handler
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void sig_handler(int signo)
{
        if (signo == SIGINT)
        {
                g_srvr->stop();
        }
}
//! ----------------------------------------------------------------------------
//! \details: Print the version.
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void print_version(FILE* a_stream, int a_exit_code)
{
        // print out the version information
        fprintf(a_stream, "http_files\n");
        fprintf(a_stream, "          Version: %s\n", IS2_VERSION);
        exit(a_exit_code);
}
//! ----------------------------------------------------------------------------
//! \details: Print the command line help.
//! \return:  TODO
//! \param:   TODO
//! ----------------------------------------------------------------------------
void print_usage(FILE* a_stream, int a_exit_code)
{
        fprintf(a_stream, "Usage: http_files [options]\n");
        fprintf(a_stream, "Options:\n");
        fprintf(a_stream, "  -h, --help          display this help and exit.\n");
        fprintf(a_stream, "  -v, --version       display the version number and exit.\n");
        fprintf(a_stream, "  -p, --port          port (default: 12345)\n");
        fprintf(a_stream, "  -b, --block_size    set block size for contiguous send/recv sizes\n");
        fprintf(a_stream, "  -r, --root          root directory (default: './')\n");
#ifdef ENABLE_PROFILER
        fprintf(a_stream, "Debug Options:\n");
        fprintf(a_stream, "  -G, --gprofile       Google cpu profiler output file\n");
        fprintf(a_stream, "  -H, --hprofile       Google heap profiler output file\n");
        fprintf(a_stream, "\n");
#endif
        exit(a_exit_code);
}
//! ----------------------------------------------------------------------------
//! main
//! ----------------------------------------------------------------------------
int main(int argc, char** argv)
{
        // -------------------------------------------------
        // defaults
        // -------------------------------------------------
        ns_is2::trc_log_level_set(ns_is2::TRC_LOG_LEVEL_ERROR);
        char l_opt;
        std::string l_arg;
        int l_option_index = 0;
        uint16_t l_port = 12345;
        uint32_t l_block_size = 4096;
        std::string l_root;
        uint32_t l_threads = 0;
#ifdef ENABLE_PROFILER
        std::string l_gprof_file;
        std::string l_hprof_file;
#endif
        // -------------------------------------------------
        // options
        // -------------------------------------------------
        struct option l_long_options[] =
                {
                // -----------------------------------------
                // options
                // -----------------------------------------
                { "help",         0, 0, 'h' },
                { "version",      0, 0, 'v' },
                { "port",         1, 0, 'p' },
                { "block_size",   1, 0, 'b' },
                { "root",         1, 0, 'r' },
#ifdef ENABLE_PROFILER
                { "gprofile",     1, 0, 'G' },
                { "hprofile",     1, 0, 'H' },
#endif

                // list sentinel
                { 0, 0, 0, 0 }
        };
        // -------------------------------------------------
        // Args...
        // -------------------------------------------------
#ifdef ENABLE_PROFILER
        char l_short_arg_list[] = "hvp:b:r:G:H:";
#else
        char l_short_arg_list[] = "hvp:b:r:";
#endif
        while ((l_opt = getopt_long_only(argc, argv, l_short_arg_list, l_long_options, &l_option_index)) != -1)
        {
                if (optarg)
                {
                        l_arg = std::string(optarg);
                }
                else
                {
                        l_arg.clear();
                }
                //NDBG_PRINT("arg[%c=%d]: %s\n", l_opt, l_option_index, l_arg.c_str());
                switch (l_opt)
                {
                // -----------------------------------------
                // *****************************************
                // options
                // *****************************************
                // -----------------------------------------
                // -----------------------------------------
                // Help
                // -----------------------------------------
                case 'h':
                {
                        print_usage(stdout, STATUS_OK);
                        break;
                }
                // -----------------------------------------
                // version
                // -----------------------------------------
                case 'v':
                {
                        print_version(stdout, STATUS_OK);
                        break;
                }
                // -----------------------------------------
                // port
                // -----------------------------------------
                case 'p':
                {
                        int l_port_val;
                        l_port_val = atoi(optarg);
                        if ((l_port_val < 1) ||
                           (l_port_val > 65535))
                        {
                                printf("Error bad port value: %d.\n", l_port_val);
                                print_usage(stdout, STATUS_ERROR);
                        }
                        l_port = (uint16_t)l_port_val;
                        break;
                }
                // -----------------------------------------
                // block_size
                // -----------------------------------------
                case 'b':
                {
                        int l_port_val;
                        l_port_val = atoi(optarg);
                        if ((l_port_val < 1))
                        {
                                printf("Error bad block size value: %d.\n", l_port_val);
                                print_usage(stdout, STATUS_ERROR);
                        }
                        l_block_size = (uint32_t)l_port_val;
                        break;
                }
                // -----------------------------------------
                // root
                // -----------------------------------------
                case 'r':
                {
                        l_root = l_arg;
                        break;
                }
#ifdef ENABLE_PROFILER
                // -----------------------------------------
                // google cpu profiler output file
                // -----------------------------------------
                case 'G':
                {
                        l_gprof_file = optarg;
                        break;
                }
#endif
#ifdef ENABLE_PROFILER
                // -----------------------------------------
                // google heap profiler output file
                // -----------------------------------------
                case 'H':
                {
                        l_hprof_file = optarg;
                        break;
                }
#endif
                // -----------------------------------------
                // What???
                // -----------------------------------------
                case '?':
                {
                        // ---------------------------------
                        // Required argument was missing:
                        // '?' is provided when the 3rd arg
                        // to getopt_long does not begin
                        // with a ':', and is preceeded by
                        // automatic error message.
                        // ---------------------------------
                        printf("  Exiting.\n");
                        print_usage(stdout, STATUS_ERROR);
                        break;
                }
                // -----------------------------------------
                // Huh???
                // -----------------------------------------
                default:
                {
                        printf("Unrecognized option.\n");
                        print_usage(stdout, STATUS_ERROR);
                        break;
                }
                }
        }
        // -------------------------------------------------
        // run server
        // -------------------------------------------------
        ns_is2::lsnr *l_lsnr = new ns_is2::lsnr(l_port, ns_is2::SCHEME_TCP);
        ns_is2::file_h *l_file_h = new ns_is2::file_h();
        if (!l_root.empty())
        {
                l_file_h->set_root(l_root);
        }
        l_lsnr->add_route("/*", l_file_h);
        ns_is2::srvr *l_srvr = new ns_is2::srvr();
        g_srvr = l_srvr;
        l_srvr->set_block_size(l_block_size);
        l_srvr->register_lsnr(l_lsnr);
        l_srvr->set_num_threads(l_threads);
        // -------------------------------------------------
        // profiling
        // -------------------------------------------------
#ifdef ENABLE_PROFILER
        if (!l_gprof_file.empty())
        {
                ProfilerStart(l_gprof_file.c_str());
        }
#endif
#ifdef ENABLE_PROFILER
        if (!l_hprof_file.empty())
        {
                HeapProfilerStart(l_hprof_file.c_str());
        }
#endif
        // -------------------------------------------------
        // Sigint handler
        // -------------------------------------------------
        if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
                printf("Error: can't catch SIGINT\n");
                return -1;
        }
        // -------------------------------------------------
        // run...
        // -------------------------------------------------
        l_srvr->run();
        // -------------------------------------------------
        // wait till done
        // -------------------------------------------------
        l_srvr->wait_till_stopped();
        // -------------------------------------------------
        // profiling
        // -------------------------------------------------
#ifdef ENABLE_PROFILER
        if (!l_gprof_file.empty())
        {
                ProfilerStop();
        }
#endif
#ifdef ENABLE_PROFILER
        if (!l_hprof_file.empty())
        {
                HeapProfilerStop();
        }
#endif
        // -------------------------------------------------
        // cleanup
        // -------------------------------------------------
        if (l_srvr) {delete l_srvr; l_srvr = NULL;}
        if (l_file_h) {delete l_file_h; l_file_h = NULL;}
        return 0;
}

