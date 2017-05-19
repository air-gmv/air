#!/usr/bin/python
##
# @package utils.asm_offsets
# @author pfnf
# @brief Generates ASM header file with the offsets provided an s file

__author__ = 'pfnf'

import re
import sys

## @brief Line parser
parse_line = re.compile(
    '\s*->\s*(?P<define>[^ ]*)\s*(?P<value>[\$$#]*[-0-9]*)\s*(?P<comment>[a-zA-Z\(\)\_,\. 0-9]*)\s*$')

## @brief Comment parser
parse_comment = re.compile('\s*->#(?P<comment>.*)\s*$')

def generate_header(input_file, output_file):

    # header
    header_file = \
        "/**\n" \
        " * @file\n" \
        " * @author pfnf\n" \
        " * @brief Memory offsets for the PMK structures\n" \
        " */\n\n" \
        "#ifndef __ASM_OFFSETS_H__\n" \
        "#define __ASM_OFFSETS_H__\n\n"

    # define
    define_file = \
        "/** @brief {0}{1} */\n" \
        "{2}{3}{4}\n"

    # comment
    comment_file = \
        "/**\n" \
        " * @defgroup asm_gp{0} {1}\n" \
        " * @{{\n" \
        " */\n"

    # footer
    footer_file = "\n#endif /* __ASM_OFFSETS_H__ */\n\n"

    # read input lines
    in_lines = []
    try:
        in_fd = open(input_file, 'r')
        in_lines = in_fd.readlines()
        in_fd.close()
    except:
        print("can't open '{0}'".format(input_file))
        return False
    #
    #try:
    out_fd = open(output_file, 'w')
    out_fd.write(header_file)

    asm_gp = 0
    in_comment = False

    for l in in_lines:

        # check if we are in a comment line
        match = parse_comment.match(l)
        if match is not None:

            # close previous comment
            if in_comment:
                out_fd.write("/** @} */\n\n")

            # write comment tag
            out_fd.write(comment_file.format(asm_gp, match.group('comment')))
            in_comment = True
            asm_gp += 1
            continue

        # define line
        match = parse_line.match(l)
        if match is not None:

            define = '#define {0}'.format(match.group('define'))
            comment = match.group('comment')
            value = match.group('value')
            space_0 = ' ' * (80 - len(comment) - 16)
            space_1 = ' ' * (80 - len(define) - len(value) - 2)

            out_fd.write(define_file.format(comment, space_0, define, space_1, value))

    # close previous comment
    if in_comment:
        out_fd.write("/** @} */\n\n")

    out_fd.write(footer_file)
    out_fd.close()

    #except:
    #    return False

    return True

if len(sys.argv) != 3:
    print('invalid number of input arguments')
    sys.exit(-1)


if not generate_header(sys.argv[1], sys.argv[2]):
    sys.exit(-1)

exit(0)




