#include <verify.hpp>
#include "vj_alignment_info.hpp"
#include "immune_gene_alignment_converter.hpp"

namespace vj_finder {
    void VJAlignmentInfo::Update(VJAlignmentInfo vj_alignment_info) {
        for(size_t i = 0; i < vj_alignment_info.NumVJHits(); i++)
            UpdateHits(vj_alignment_info.GetVJHitsByIndex(i));
        for(size_t i = 0; i < vj_alignment_info.NumFilteredReads(); i++)
            UpdateFilteringInfo(vj_alignment_info.GetFilteringInfoByIndex(i));
    }

    void VJAlignmentInfo::UpdateFilteringInfo(VJFilteringInfo filtering_info) {
        filtering_infos_.push_back(filtering_info);
        read_id_filtering_info_map_[filtering_info.read->id] = filtering_infos_.size() - 1;
    }

    void VJAlignmentInfo::UpdateChainTypeMap(const VJHits &vj_hits) {
        auto chain_type = vj_hits.GetVHitByIndex(0).ImmuneGene().Chain();
        if(chain_type_abundance_.find(chain_type) == chain_type_abundance_.end())
            chain_type_abundance_[chain_type] = 0;
        chain_type_abundance_[chain_type]++;
    }

    void VJAlignmentInfo::UpdateHits(VJHits vj_hits) {
        UpdateChainTypeMap(vj_hits);
        alignment_records_.push_back(std::move(vj_hits));
        read_id_hit_index_map_[vj_hits.Read().id] = alignment_records_.size() - 1;
    }


    const VJFilteringInfo VJAlignmentInfo::GetFilteringInfoByIndex(size_t index) const {
        VERIFY_MSG(index < NumFilteredReads(), "Index " << index << " exceeds number of filtered reads " <<
                                               NumFilteredReads());
        return filtering_infos_[index];
    }

    const VJHits& VJAlignmentInfo::GetVJHitsByIndex(size_t index) const {
        VERIFY_MSG(index < NumVJHits(), "Index " << index << " exceeds number of VJ hits " <<
                                               NumVJHits());
        return alignment_records_[index];
    }

    const VJHits& VJAlignmentInfo::GetVJHitsByRead(const core::Read &read) const {
        VERIFY_MSG(read_id_hit_index_map_.find(read.id) != read_id_hit_index_map_.end(),
                   "Alignment info does not contain read " << read.name);
        return alignment_records_[read_id_hit_index_map_.at(read.id)];
    }

    VJFilteringInfo VJAlignmentInfo::GetFilteringInfoByRead(const core::Read &read) const {
        VERIFY_MSG(read_id_filtering_info_map_.find(read.id) != read_id_filtering_info_map_.end(),
                   "Alignment info does not contain read " << read.name);
        return filtering_infos_[read_id_filtering_info_map_.at(read.id)];
    }

    void VJAlignmentOutput::OutputAlignmentInfo() const {
        std::ofstream out(output_params_.output_files.alignment_info_fname);
        const auto columns = ReportColumns::ColumnSet<ReportFeatureEvaluationContext, std::ofstream>::ParseColumns(output_params_.output_details.alignment_columns);
        columns.PrintCsvHeader(out);
        for(size_t i = 0; i < alignment_info_.NumVJHits(); i++) {
            const auto vj_hits = alignment_info_.GetVJHitsByIndex(i);
            for(size_t j = 0; j < output_params_.output_details.num_aligned_candidates; j++) {
                const auto v_hits = vj_hits.GetVHitByIndex(j);
                const auto j_hits = vj_hits.GetJHitByIndex(j);
                columns.print(out, ReportFeatureEvaluationContext{vj_hits, v_hits, j_hits});
            }
        }
        out.close();
        INFO("Alignment info was written to " << output_params_.output_files.alignment_info_fname);
    }

    void VJAlignmentOutput::OutputCleanedReads() const {
        std::ofstream out(output_params_.output_files.cleaned_reads_fname);
        for(size_t i = 0; i < alignment_info_.NumVJHits(); i++) {
            auto read = alignment_info_.GetVJHitsByIndex(i).Read();
            out << ">" << read.name << std::endl;
            out << read.seq << std::endl;
        }
        out.close();
        INFO("Cleaned reads were written to " << output_params_.output_files.cleaned_reads_fname);
    }

    void VJAlignmentOutput::OutputFilteredReads() const {
        std::ofstream out(output_params_.output_files.filtered_reads_fname);
        for(size_t i = 0; i < alignment_info_.NumFilteredReads(); i++) {
            auto read = alignment_info_.GetFilteredReadByIndex(i);
            out << ">" << read.name << std::endl;
            out << read.seq << std::endl;
        }
        out.close();
        INFO("Filtered reads were written to " << output_params_.output_files.filtered_reads_fname);
    }

    void VJAlignmentOutput::OutputVAlignments() const {
        ImmuneGeneAlignmentConverter alignment_converter;
        std::ofstream out(output_params_.output_files.valignments_filename);
        for(size_t i = 0; i < alignment_info_.NumVJHits(); i++) {
            auto vj_hits = alignment_info_.GetVJHitsByIndex(i);
            auto v_hit = vj_hits.GetVHitByIndex(0);
            auto v_alignment = alignment_converter.ConvertToAlignment(v_hit.ImmuneGene(), vj_hits.Read(),
                                                                      v_hit.BlockAlignment());
            auto subject_row = seqan::row(v_alignment.Alignment(), 0);
            auto query_row = seqan::row(v_alignment.Alignment(), 1);
            out << ">INDEX:" << i + 1 << "|READ:" << vj_hits.Read().name << "|START_POS:" <<
                    v_alignment.StartSubjectPosition() << "|END_POS:" <<
                    v_alignment.EndSubjectPosition() << std::endl;
            out << query_row << std::endl;
            out << ">INDEX:" << i + 1 << "|GENE:" << v_alignment.subject().name() <<
            "|START_POS:" << v_alignment.StartQueryPosition() << "|END_POS:" <<
                    v_alignment.EndQueryPosition() << "|CHAIN_TYPE:" <<
                    v_alignment.subject().Chain() << std::endl;
            out << subject_row << std::endl;
        }
        out.close();
        INFO("V alignments were written to " << output_params_.output_files.valignments_filename);
    }

    void VJAlignmentOutput::OutputFilteringInfo() const {
        std::ofstream out(output_params_.output_files.filtering_info_filename);
        for(size_t i = 0; i < alignment_info_.NumFilteredReads(); i++) {
            auto filtering_info = alignment_info_.GetFilteringInfoByIndex(i);
            out << filtering_info.read->name << "\t" << filtering_info << std::endl;
        }
        out.close();
        INFO("Information about filtered reads was written to " << output_params_.output_files.filtering_info_filename);
    }
}

namespace ReportColumns {
    using VJFReportColumn = ReportColumns::Column<vj_finder::ReportFeatureEvaluationContext, std::ofstream>;

    template <>
    const std::vector<ReportColumns::VJFReportColumn> ReportColumns::VJFReportColumn::COLUMN_TYPES = {
            {"Read_name", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.vj_hits.Read().name; }},
            {"Chain_type", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.v_hits.ImmuneGene().Chain(); }},
            {"V_hit", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.v_hits.ImmuneGene().name(); }},
            {"V_start_pos", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.v_hits.FirstMatchReadPos() + 1; }},
            {"V_end_pos", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.v_hits.LastMatchReadPos(); }},
            {"V_score", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.v_hits.Score(); }},
            {"J_hit", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.j_hits.ImmuneGene().name(); }},
            {"J_start_pos", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.j_hits.FirstMatchReadPos() + 1; }},
            {"J_end_pos", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.j_hits.LastMatchReadPos(); }},
            {"J_score", [](std::ofstream& out, const vj_finder::ReportFeatureEvaluationContext& context) { out << context.j_hits.Score(); }}
    };

    using VJFReportColumnSet = ReportColumns::ColumnSet<vj_finder::ReportFeatureEvaluationContext, std::ofstream>;

    template <>
    const std::vector<ReportColumns::VJFReportColumnSet> ReportColumns::VJFReportColumnSet::PRESETS = {};
}