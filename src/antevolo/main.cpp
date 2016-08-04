#include <logger/logger.hpp>
#include <logger/log_writers.hpp>
#include <verify.hpp>
#include <segfault_handler.hpp>
#include <perfcounter.hpp>

#include <copy_file.hpp>

#include "antevolo_config.hpp"
#include "antevolo_launch.hpp"
#include <clonally_related_candidates_calculators/edmonds_tarjan_DMST_calculator.hpp>

void create_console_logger(/*std::string cfg_filename*/) {
    using namespace logging;
    std::string log_props_file = "";
    //if (!path::FileExists(log_props_file)){
    //    log_props_file = path::append_path(path::parent_path(cfg_filename), log_props_file);
    //}
    logger *lg = create_logger(path::FileExists(log_props_file) ? log_props_file : "");
    lg->add_writer(std::make_shared<console_writer>());
    attach_logger(lg);
}

std::string running_time_format(const perf_counter &pc) {
    unsigned ms = (unsigned)pc.time_ms();
    unsigned secs = (ms / 1000) % 60;
    unsigned mins = (ms / 1000 / 60) % 60;
    unsigned hours = (ms / 1000 / 60 / 60);
    boost::format bf("%u hours %u minutes %u seconds");
    bf % hours % mins % secs;
    return bf.str();
}

void prepare_output_dir(const antevolo::AntEvoloConfig::OutputParams & op) {
    path::make_dir(op.output_dir);
    path::make_dir(op.tree_dir);
    path::make_dir(op.vertex_dir);
    path::make_dir(op.cdr_graph_dir);
}

void copy_configs(std::string cfg_filename, std::string to) {
    path::make_dir(to);
    path::copy_files_by_ext(path::parent_path(cfg_filename), to, ".info", true);
    path::copy_files_by_ext(path::parent_path(cfg_filename), to, ".properties", true);
}

std::string get_config_fname(int argc, char **argv) {
    if(argc == 2 and (std::string(argv[1]) != "--help" and std::string(argv[1]) != "--version" and
            std::string(argv[1]) != "--help-hidden"))
        return std::string(argv[1]);
    return "configs/antevolo/config.info";
}


antevolo::AntEvoloConfig load_config(int argc, char **argv) {
    std::string cfg_filename = get_config_fname(argc, argv);
    antevolo::AntEvoloConfig antevolo_config;
    antevolo_config.load(cfg_filename);
    prepare_output_dir(antevolo_config.output_params);
    std::string path_to_copy = path::append_path(antevolo_config.output_params.output_dir, "configs");
    path::make_dir(path_to_copy);
    copy_configs(cfg_filename, path_to_copy);
    return antevolo_config;
}


int main(int argc, char **argv) {

    segfault_handler sh;
    perf_counter pc;
    create_console_logger();
    /*
    using namespace antevolo;
    std::ifstream in("/home/aslabodkin/trees/Edmonds_test.txt");
    typedef EdmondsTarjanDMSTCalculator::WeightedEdge WeightedEdge;
    std::vector<WeightedEdge> edges;
    size_t E;
    in >> E;
    size_t V = 0;
    for (size_t i = 0; i < E; ++i) {
        size_t src;
        size_t dst;
        double weight;
        in >> src >> dst >> weight;
        edges.push_back(WeightedEdge(src, dst, weight));
        V = std::max(V, src);
        V = std::max(V, dst);
    }
    V++;
    EdmondsTarjanDMSTCalculator e_calc(V, edges, 0);
    e_calc.EmpondsTarjan();
    INFO("Minimal arborescence found");
    std::vector<WeightedEdge> res = e_calc.GetParentEdges();
    for (size_t i = 1; i < V; ++i)
    {
        std::cout << res[i].src_ << " " << res[i].dst_ << " " << res[i].weight_ << std::endl;
    }
    */
    antevolo::AntEvoloConfig config = load_config(argc, argv);
    antevolo::AntEvoloLaunch(config).Launch();

    //cdr_labeler::CDRLabelerLaunch(config).Launch();
    return 0;
}