import xlsxwriter

def create_spreadsheet():
    workbook = xlsxwriter.Workbook('low-level-memory-simulator.xlsx')

    # Formats
    header_format = workbook.add_format({'bold': True, 'bg_color': '#D3D3D3', 'border': 1})
    cell_format = workbook.add_format({'border': 1})
    highlight_format = workbook.add_format({'bg_color': '#FFFF00', 'border': 1})  # Yellow
    pad_format = workbook.add_format({'bg_color': '#FFCCCC', 'border': 1})  # Light red for padding
    data_format_char = workbook.add_format({'bg_color': '#ADD8E6', 'border': 1})  # Light blue
    data_format_int = workbook.add_format({'bg_color': '#90EE90', 'border': 1})   # Light green
    data_format_short = workbook.add_format({'bg_color': '#DDA0DD', 'border': 1}) # Plum/Purple

    # ---------------------------------------------------------
    # Tab 1: Array Offsets
    # ---------------------------------------------------------
    ws1 = workbook.add_worksheet('1_Array_Offsets')

    # Set column widths
    ws1.set_column('A:A', 20)
    ws1.set_column('B:D', 18)

    # Named ranges
    workbook.define_name('BaseAddress', '=1_Array_Offsets!$B$3')
    workbook.define_name('ElementWidth', '=1_Array_Offsets!$B$4')
    workbook.define_name('TargetIndex', '=1_Array_Offsets!$B$5')

    # Inputs
    ws1.write('A3', 'Base Address:', header_format)
    ws1.write('B3', 10000, cell_format)

    ws1.write('A4', 'Element Width:', header_format)
    ws1.write('B4', 4, cell_format)

    ws1.write('A5', 'Target Index:', header_format)
    ws1.write('B5', 2, cell_format)
    ws1.data_validation('B5', {'validate': 'integer',
                               'criteria': 'between',
                               'minimum': 0,
                               'maximum': 5,
                               'error_message': 'Error: Segmentation Fault (Core Dumped)'})

    ws1.write('A7', 'Target Address:', header_format)
    ws1.write_formula('B7', '=BaseAddress + (ElementWidth * TargetIndex)', cell_format)

    ws1.write('A8', 'Target Value:', header_format)
    ws1.write_formula('B8', '=OFFSET(D12, ElementWidth * TargetIndex, 0)', cell_format)

    # Table headers
    headers = ['Address', 'Offset (Bytes)', 'Index', 'Value (32-bit Int)']
    for col_num, data in enumerate(headers):
        ws1.write(10, col_num, data, header_format)

    # Memory layout rows
    # We will do 24 bytes (rows 12 to 35 in Excel terms, index 11 to 34 in xlsxwriter)
    for i in range(24):
        row = 11 + i
        offset = i
        ws1.write_formula(row, 0, f'=BaseAddress + {offset}', cell_format)
        ws1.write(row, 1, offset, cell_format)
        ws1.write_formula(row, 2, f'=IF(MOD({offset}, ElementWidth)=0, {offset}/ElementWidth, "")', cell_format)
        
        # Give some values for testing
        val = ""
        if offset % 4 == 0:
            val = (offset // 4) * 100 + 100
        ws1.write(row, 3, val, cell_format)

    # Conditional Formatting
    # Highlight row if $B12 = (ElementWidth * TargetIndex)
    ws1.conditional_format('A12:D35', {'type': 'formula',
                                       'criteria': '=$B12=(ElementWidth*TargetIndex)',
                                       'format': highlight_format})


    # ---------------------------------------------------------
    # Tab 2: Struct Padding
    # ---------------------------------------------------------
    ws2 = workbook.add_worksheet('2_Struct_Padding')

    ws2.set_column('A:A', 15)
    ws2.set_column('B:B', 15)
    ws2.set_column('C:C', 30)
    ws2.set_column('D:D', 30)

    ws2.write('A1', 'Visualizing Struct Alignment & Padding', header_format)
    ws2.write('A3', 'Address', header_format)
    ws2.write('B3', 'Offset', header_format)
    ws2.write('C3', 'Variable', header_format)
    ws2.write('D3', 'Byte Role', header_format)

    base_addr = 10000
    layout = [
        ('student.id', 'Data (Char)', data_format_char),
        ('None', 'Padding', pad_format),
        ('None', 'Padding', pad_format),
        ('None', 'Padding', pad_format),
        ('student.grade', 'Data (Int - Byte 0)', data_format_int),
        ('student.grade', 'Data (Int - Byte 1)', data_format_int),
        ('student.grade', 'Data (Int - Byte 2)', data_format_int),
        ('student.grade', 'Data (Int - Byte 3)', data_format_int),
        ('student.age', 'Data (Short - Byte 0)', data_format_short),
        ('student.age', 'Data (Short - Byte 1)', data_format_short),
        ('None', 'Padding', pad_format),
        ('None', 'Padding', pad_format),
    ]

    for i, (var, role, fmt) in enumerate(layout):
        row = 3 + i
        ws2.write(row, 0, base_addr + i, cell_format)
        ws2.write(row, 1, i, cell_format)
        ws2.write(row, 2, var, fmt)
        ws2.write(row, 3, role, fmt)


    # ---------------------------------------------------------
    # Tab 3: Endianness Flip
    # ---------------------------------------------------------
    ws3 = workbook.add_worksheet('3_Endianness_Flip')

    ws3.set_column('A:E', 25)

    workbook.define_name('EndianMode', '=3_Endianness_Flip!$B$2')

    ws3.write('A2', 'Endian Mode:', header_format)
    ws3.write('B2', 'Big Endian', cell_format)
    ws3.data_validation('B2', {'validate': 'list', 'source': ['Big Endian', 'Little Endian']})

    ws3.write('A4', 'Value in Memory: 0x12345678', header_format)

    headers3 = ['Address', 'Offset', 'Byte Value Formula', 'Result (Big Endian)', 'Result (Little Endian)']
    for col_num, data in enumerate(headers3):
        ws3.write(5, col_num, data, header_format)

    # Addresses
    base_addr = 10000

    # Big Endian expected: 12, 34, 56, 78
    # Little Endian expected: 78, 56, 34, 12
    byte_pairs = [("12", "78"), ("34", "56"), ("56", "34"), ("78", "12")]

    for i in range(4):
        row = 6 + i
        ws3.write(row, 0, base_addr + i, cell_format)
        ws3.write(row, 1, i, cell_format)
        big, little = byte_pairs[i]
        formula = f'=IF(EndianMode="Big Endian", "{big}", "{little}")'
        ws3.write_formula(row, 2, formula, cell_format)
        ws3.write(row, 3, big, cell_format)
        ws3.write(row, 4, little, cell_format)

    workbook.close()

if __name__ == '__main__':
    create_spreadsheet()
