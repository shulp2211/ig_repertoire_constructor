#include <gtest/gtest.h>

#include "ig_block_alignment.hpp"

using namespace fast_ig_tools;

TEST(block_chain_alignment_test, find_simple_gap_test) {
    EXPECT_EQ(1, find_simple_gap("A", "AC"));
    EXPECT_EQ(0, find_simple_gap("C", "AC"));
    EXPECT_EQ(1, find_simple_gap("AA", "ACA"));
    EXPECT_EQ(2, find_simple_gap("AC", "ACT"));
    EXPECT_EQ(0, find_simple_gap("CA", "ACA"));
    EXPECT_EQ(0, find_simple_gap("CAAAA", "ACAAAA"));
    EXPECT_EQ(3, find_simple_gap("ACAACA", "ACAXXXAXA"));
    EXPECT_EQ(0, find_simple_gap("", "ACAXXXAXA"));
    EXPECT_EQ(0, find_simple_gap("", "A"));
    EXPECT_EQ(0, find_simple_gap("", "GACTGGTTGG"));
    EXPECT_TRUE(false);
}

/*
TACCACGATATTTTGACTGGTTGGGACTACTGGGGCCAGGGAACCCTGGTCACCGTCTCCTCA
ACTACTTTGACTACTGGGGCCAGGGAACCCTGGTCACCGTCTCCTCAG
{0}(5)3(0+10)39(0){1}
      0     .    :    .    :    .    :    .    :    .    :
        ------AC----------TACTTTGACTACTGGGGCCAGGGAACCCTGGT
                             |  ||||||||||||||||||||||||||
        TACCACGATATTTTGACTGGTTGGGACTACTGGGGCCAGGGAACCCTGGT
     50     .    :
        CACCGTCTCCTCAG
        |||||||||||||
        CACCGTCTCCTCA-


      ACTACTTT          GACTACTGGGGCCAGGGAACCCTGGTCACCGTCTCCTCAG
TACCACGATATTTTGACTGGTTGGGACTACTGGGGCCAGGGAACCCTGGTCACCGTCTCCTCA

GAAGTGCAGCTGGTGGAGTCTGGGGGAGGCTTGGTCCAGCCTGGGGGGTCCCTGAGACTCTCCTGCGCAGCCTCTGGGTTCACCGTCAGTAGTAATTACTTGAGTTGGGTCCGCCAGGCTCCAGGGAAGGGGCTGGAGTGGGTCTCTGGTATTAATTGGAATGGTGGTAGCACAGGTTATGCAGACTCTGTGAAGGGCCGATTCACCATCTCCAGGGACGACGCCAGGAATTCAATGTATCTGCAAATGAACAGCCTGAGAGTCGAGGACACGGCTGTGTATTACTGTGCGAGAGATCAGTATTACTATGATAGTAGTGGTTATTACCTCTGGGGCCAGGGAACCCTGGTCACCGTCTCCTCA
GAGGTGCAGCTGGTGGAGTCTGGGGGAGGCTTGGTCCAGCCTGGGGGGTCCCTGAGACTCTCCTGTGCAGCCTCTGGATTCACCGTCAGTAGCAACTACATGAGCTGGGTCCGCCAGGCTCCAGGGAAGGGGCTGGAGTGGGTCTCAGTTATTTATAGCGGTGGTAGCACATACTACGCAGACTCCGTGAAGGGCAGATTCACCATCTCCAGAGACAATTCCAAGAACACGCTGTATCTTCAAATGAACAGCCTGAGAGCCGAGGACACGGCTGTGTATTACTGTGCGAGAGA
{0}(3)62(1)11(1)14(13)41(13+3)12(6)8(1)9(1)16(20)7(1)19(1)33(0){0}


 */
