<%
    import xky
    import utils.templates as makoutils
%>\
${makoutils.cfileHeader('usr_arch.c', 'SPARC specific configuration')}

#include <cpu.h>

/**
 * @brief SPARC specific configuration
 */
arch_configuration_t arch_configuration = { ${'\\'}
    .mmu_context_entries   = ${configuration.arch.mmu_context_entries},
    .mmu_l1_tables_entries = ${configuration.arch.mmu_l1_tables_entries},
    .mmu_l2_tables_entries = ${configuration.arch.mmu_l2_tables_entries},
    .mmu_l3_tables_entries = ${configuration.arch.mmu_l3_tables_entries},
};
