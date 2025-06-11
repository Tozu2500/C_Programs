#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// ----------------------------------------------------------------------------------
// ARCHITECTURAL CONSTANTS AND CONFIGURATION PARAMETERS
// These constants define the operational boundaries and behavioral
// parameters of the text processing engine, enabling fine-tuned performance
// optimization and memory management strategies across different deployemnt scenarios.
// ----------------------------------------------------------------------------------

#define MAX_LINE_LENGTH 4096 // Max characters per processed line
#define MAX_WORD_LENGTH 256 // Max length for individual words
#define MAX_DICTIONARY_SIZE 10000 // Max vocabulary entries
#define HASH_TABLE_SIZE 1009 // Prime number for optimal hash distribution
#define INITIAL_BUFFER_SIZE 1024 // Starting buffer size for dynamic allocation
#define BUFFER_GROWTH_FACTOR 2 // Multiplicative factor for buffer expansion

// ----------------------------------------------------------------------------------
// SOPHISTICATED ENUM TYPES FOR OPERATIONAL MODES
// These enums provide type-safe config options that control various aspects
// of the text processing behavior, enabling compile-time optimization and runtime
// flexibility in processing strategies.
// ----------------------------------------------------------------------------------

typedef enum {
    MODE_BASIC_ANALYSIS, // Fundamental statistical analysis
    MODE_ADVANCED_PARSING, // Complex linguistic pattern recognition
    MODE_SEMANTIC_PROCESSING, // Context-aware semantic interpretation
    MODE_PERFORMANCE_OPTIMIZED // High-throughput processing mode
} TextProcessingMode;

/* 
 * ProcessingStatus: Comprehensive status tracking enumeration that enables
 * detailed error handling, operational monitoring and debugging capabilities
 * throughout the entire text processing pipeline.
*/
typedef enum {
    STATUS_SUCCESS = 0, // Operation completed successfully
    STATUS_ERROR_MEMORY, // Memory allocation failure
    STATUS_ERROR_FILE_TO, // File I/O error
    STATUS_ERROR_INVALID_INPUT, // Input invalid
    STATUS_ERROR_BUFFER_OVERFLOW, // Buffer capacity overflow
    STATUS_ERROR_PROCESSING, // General processing error
    STATUS_WARNING_TRUNCATED // Data was truncated and processing continued
} ProcessingStatus;

//--------------------------------------------------------------------------
// ADVANCED DATA STRUCTURES FOR COMPREHENSIVE TEXT ANALYSIS
// These structures implement sophisticated data organization patterns that
// enable efficient storage, retrieval, and manipulation of complex textual
// information while maintaining optimal memory utilization and access patterns.
//---------------------------------------------------------------------------

/**
 * WordStatistics: Comprehensive statistical analysis container that tracks
 * detailed metrics for individual words, enabling sophisticated frequency
 * analysis, linguistic pattern recognition and contextual understanding
 * of textual content with high precision and efficiency.
 */
typedef struct {
    char word[MAX_WORD_LENGTH]; // The actual word string
    unsigned int frequency; // Frequency count in processed text
    unsigned int line_occurrences; // Number of lines containing this word
    double relative_frequency; // Normalized frequency as percentage
    bool is_capitalized; // First occurrence capitalization status
    unsigned int first_position; // Character position of first occurrence
    unsigned int last_position; // Character position of last occurrence
} WordStatistics;

/**
 * TextMetrics: Holistic document analysis structure containing comprehensive
 * statistical measurements and linguistic characteristics that provide deep insights
 * into textual composition, readability, and structural properties that are
 * essentail for advanced text processing.
 */
typedef struct {
    unsigned long total_characters; // Total character count including spaces
    unsigned long total_words; // Total word count
    unsigned long total_lines; // Total line count
    unsigned long unique_words; // Unique word count
    unsigned long sentences; // Sentence count
    unsigned long paragraphs; // Paragraph count
    double average_word_length; // Average characters per word
    double average_words_per_line; // Average words per line
    double vocabulary_richness; // Unique words
    double readability_score; // Computed readability metric
} TextMetrics;

/**
 * HashEntry: Sophisticated hash table entry implementing chaining collision
 * resolution with embedded statistical tracking and memory-efficient storage
 * for high-performance word frequency analysis and dictionary operations.
 */
typedef struct HashEntry {
    WordStatistics *word_stats; // Pointer for detailed word stats
    struct HashEntry *next; // Linked list chain for collision resolution
    unsigned int hash_value; // Cached hash value for optimization
} HashEntry;

/**
 * TextProcessor: Master control structure implementing the complete text
 * processing engine with configurable optional parameters, great memory management,
 * comprehensive stats tracking and extensible architecture supporting multiple processing modes and strategies.
 */
typedef struct {
    HashEntry *word_table[HASH_TABLE_SIZE]; // Hash table for word tracking
    TextMetrics metrics; // Comprehensive doc. metrics
    TextProcessingMode mode; // Current processing mode
    char *input_buffer; // Dynamic input buffer
    size_t buffer_capacity; // Current size of the buffer
    size_t buffer_used; // Current buffer usage
    FILE *input_file; // Input file handle
    FILE *output_file; // Output file handle
    bool verbose_mode; // Detailed logging flag
    bool case_sensitive; // Case sensitivity option
    unsigned int processing_flags; // Bitfield for processing options
} TextProcessor;

//-----------------------------------------------------------------------------------
/**
 * SOPHISTICATED FUNCTION POINTER TYPEDEFS FOR EXTENSIBLE ARCH
 */
typedef ProcessingStatus (*ProcessingCallback)(TextProcessor *processor,
                                                const char *input_data,
                                                size_t data_length);

typedef bool (*WordFilterCallback)(const char *word, size_t word_length);

// CORE ENGINE MANAGEMENT FUNCTIONS
ProcessingStatus processor_initialize(TextProcessor **processor, TextProcessingMode mode);

ProcessingStatus processor_configure(TextProcessor *processor, unsigned int flags);

ProcessingStatus processor_destroy(TextProcessor *processor);

// FILE INPUT AND OUTPUT BUFFER MANAGEMENT
ProcessingStatus processor_load_file(TextProcessor *processor, const char *filename);
ProcessingStatus processor_process_string(TextProcessor *processor, const char *input_text);
ProcessingStatus processor_set_output_file(TextProcessor *processor, const char *filename);

// Advanced text analysis operations
ProcessingStatus processor_analyze_text(TextProcessor *processor);
ProcessingStatus processor_generate_statistics(TextProcessor *processor);
ProcessingStatus processor_export_results(TextProcessor *processor, const char *output_format);

// Hash table and dictionary ops
unsigned int hash_function(const char *key);
ProcessingStatus hash_table_insert(TextProcessor *processor,
                                const char *word,
                                unsigned int line_number,
                                unsigned int position);
WordStatistics* hash_table_lookup(TextProcessor *processor, const char *word);

// Util and helper functions
void print_comprehensive_statistics(const TextProcessor *processor);
void print_word_frequency_report(const TextProcessor *processor);
ProcessingStatus validate_input_parameters(const char *input, size_t length);
char* normalize_word(const char *input_word, bool case_sensitive);

#endif // TEXT_PROCESSOR_H