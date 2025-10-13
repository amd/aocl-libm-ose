#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
AMD Testsuite Parser
Cross-platform compatible for Windows and Linux
Extracts IP, OP, ULP, Status, and Duration from YAML dump files
Generates text output with parallel display and interactive plots
"""

import re
import sys
import struct
import os
import platform
import warnings
from datetime import datetime

# Suppress specific numpy warnings about subnormal values
warnings.filterwarnings("ignore", message="The value of the smallest subnormal")

# Try to import plotting libraries
PLOTTING_AVAILABLE = False
try:
    import matplotlib.pyplot as plt
    import numpy as np
    PLOTTING_AVAILABLE = True
    print("Plotting libraries loaded successfully")
except ImportError:
    print("Warning: Matplotlib/numpy not available. Plotting will be disabled.")



class EnhancedAMDDumpParser:
    """
    Enhanced AMD Dump Parser for extracting and analyzing data from AMD Testsuite YAML dumps.
    Features include:
    - Initialization
    - IEEE 754 conversion
    - Formatting utilities
    - Core YAML parsing
    - File processing
    - Visualization
    - Statistical analysis
    - Text output
    - HTML summaries
    """

    def __init__(self):
        self.all_ips = []
        self.all_ops = []
        self.all_ulps = []
        self.all_status = []
        self.all_durations = []
        self.is_perf_test = False
        self.n_value = 1

    def hex_to_float(self, hex_str):
        """Convert hexadecimal string to float value"""
        try:
            if isinstance(hex_str, str) and hex_str.startswith('0x'):
                hex_value = hex_str[2:]
                if len(hex_value) <= 8:  # 32-bit float
                    int_val = int(hex_str, 16)
                    return struct.unpack('f', struct.pack('I', int_val))[0]
                else:  # 64-bit double
                    int_val = int(hex_str, 16)
                    return struct.unpack('d', struct.pack('Q', int_val))[0]
            elif isinstance(hex_str, (int, float)):
                return float(hex_str)
            else:
                return float(hex_str)
        except:
            return None

    def hex_to_decimal_str(self, hex_str):
        """Convert hex to floating-point string representation"""
        try:
            if isinstance(hex_str, str) and hex_str.startswith('0x'):
                hex_value = hex_str[2:]
                if len(hex_value) <= 8:  # 32-bit float
                    int_val = int(hex_str, 16)
                    float_val = struct.unpack('f', struct.pack('I', int_val))[0]
                    # Handle special cases
                    if float_val == float('inf'):
                        return "inf"
                    elif float_val == float('-inf'):
                        return "-inf"
                    elif float_val != float_val:  # NaN check
                        return "nan"
                    else:
                        return str(float_val)
                else:  # 64-bit double
                    int_val = int(hex_str, 16)
                    double_val = struct.unpack('d', struct.pack('Q', int_val))[0]
                    # Handle special cases
                    if double_val == float('inf'):
                        return "inf"
                    elif double_val == float('-inf'):
                        return "-inf"
                    elif double_val != double_val:  # NaN check
                        return "nan"
                    else:
                        return str(double_val)
            return str(hex_str)
        except:
            return str(hex_str)

    def parse_yaml_dump_line(self, line):
        """Parse a single line from YAML dump file"""
        try:
            line = line.strip()
            if line.startswith('{') and line.endswith('}'):
                line = line[1:-1]

            data = {}
            parts = []
            current_part = ""
            bracket_count = 0

            for char in line:
                if char in '([':
                    bracket_count += 1
                elif char in ')]':
                    bracket_count -= 1
                elif char == ',' and bracket_count == 0:
                    parts.append(current_part.strip())
                    current_part = ""
                    continue
                current_part += char

            if current_part.strip():
                parts.append(current_part.strip())

            for part in parts:
                if ':' in part:
                    key, value = part.split(':', 1)
                    key = key.strip()
                    value = value.strip()

                    if value.startswith('[') and value.endswith(']'):
                        array_content = value[1:-1]
                        if array_content.startswith('[') and array_content.endswith(']'):
                            nested_arrays = []
                            current_array = ""
                            bracket_count = 0

                            for char in array_content:
                                if char == '[':
                                    bracket_count += 1
                                elif char == ']':
                                    bracket_count -= 1
                                    if bracket_count == 0:
                                        current_array += char
                                        nested_arrays.append(current_array)
                                        current_array = ""
                                        continue
                                elif char == ',' and bracket_count == 0:
                                    continue
                                current_array += char

                            parsed_arrays = []
                            for arr in nested_arrays:
                                if arr.startswith('[') and arr.endswith(']'):
                                    inner_content = arr[1:-1]
                                    elements = [elem.strip() for elem in inner_content.split(',')]
                                    parsed_arrays.append(elements)
                            data[key] = parsed_arrays
                        else:
                            elements = [elem.strip() for elem in array_content.split(',')]
                            data[key] = elements
                    else:
                        data[key] = value

            # Extract n value if present
            if 'n' in data:
                try:
                    self.n_value = int(data['n'])
                except:
                    self.n_value = 1

            return data
        except Exception as e:
            print("Error parsing line: {}... Error: {}".format(line[:50], str(e)))
            return None

    def extract_data_from_file(self, file_path):
        """Extract all data from dump file"""
        print("Reading file: {}".format(file_path))
        
        # Store the input file path for function name detection
        self.input_file_path = file_path

        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
                lines = file.readlines()

            for line_num, line in enumerate(lines, 1):
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                parsed_data = self.parse_yaml_dump_line(line)
                if parsed_data:
                    # Extract IP values
                    if 'ip' in parsed_data and parsed_data['ip']:
                        ip_data = parsed_data['ip']
                        if isinstance(ip_data, list) and len(ip_data) > 0:
                            if isinstance(ip_data[0], list):
                                for ip_group in ip_data:
                                    self.all_ips.extend(ip_group)
                            else:
                                self.all_ips.extend(ip_data)

                    # Extract OP values
                    if 'op' in parsed_data and parsed_data['op']:
                        op_data = parsed_data['op']
                        if isinstance(op_data, list) and len(op_data) > 0:
                            if isinstance(op_data[0], list):
                                for op_group in op_data:
                                    self.all_ops.extend(op_group)
                            else:
                                self.all_ops.extend(op_data)

                    # Extract ULP values
                    if 'ulp' in parsed_data and parsed_data['ulp']:
                        ulp_data = parsed_data['ulp']
                        if isinstance(ulp_data, list) and len(ulp_data) > 0:
                            if isinstance(ulp_data[0], list):
                                for ulp_group in ulp_data:
                                    self.all_ulps.extend(ulp_group)
                            else:
                                self.all_ulps.extend(ulp_data)

                    # Extract Status values
                    if 'status' in parsed_data and parsed_data['status']:
                        status_data = parsed_data['status']
                        if isinstance(status_data, list) and len(status_data) > 0:
                            if isinstance(status_data[0], list):
                                for status_group in status_data:
                                    self.all_status.extend(status_group)
                            else:
                                self.all_status.extend(status_data)

                    # Extract Duration values
                    if 'duration' in parsed_data and parsed_data['duration']:
                        duration_data = parsed_data['duration']
                        if isinstance(duration_data, list):
                            self.all_durations.extend(duration_data)
                        else:
                            self.all_durations.append(duration_data)
                        self.is_perf_test = True

            print("Extraction complete!")
            print("- IP values: {}".format(len(self.all_ips)))
            print("- OP values: {}".format(len(self.all_ops)))
            print("- ULP values: {}".format(len(self.all_ulps)))
            print("- Status values: {}".format(len(self.all_status)))
            print("- Duration values: {}".format(len(self.all_durations)))
            print("- Vectorization size (n): {}".format(self.n_value))

        except Exception as e:
            print("Error reading file {}: {}".format(file_path, str(e)))

    def generate_plots(self, output_prefix, plots_dir):
        """Generate visualization plots in organized directory"""
        if not PLOTTING_AVAILABLE:
            print("Plotting disabled - matplotlib not available")
            return []

        if not self.all_ips or not self.all_ulps:
            print("No data available for plotting")
            return []

        # Create plots directory (cross-platform)
        if not os.path.exists(plots_dir):
            os.makedirs(plots_dir)

        plot_files = []

        # Convert hex values to float for plotting
        print("Converting hex values to float for plotting...")
        ip_floats = []
        ulp_floats = []
        status_data = []

        min_len = min(len(self.all_ips), len(self.all_ulps), len(self.all_status))

        for i in range(min_len):
            ip_float = self.hex_to_float(self.all_ips[i])
            ulp_float = self.hex_to_float(self.all_ulps[i])

            if ip_float is not None and ulp_float is not None:
                # Filter out inf and nan values
                if PLOTTING_AVAILABLE and not (np.isinf(ip_float) or np.isnan(ip_float) or
                       np.isinf(ulp_float) or np.isnan(ulp_float)):
                    # Convert input to log10(abs(ip)) for plotting (matching the graph style)
                    if abs(ip_float) > 0:  # Avoid log(0)
                        ip_log = np.log10(abs(ip_float))  # Use log10 instead of natural log
                        ip_floats.append(ip_log)
                        ulp_floats.append(abs(ulp_float))  # Use absolute ULP
                        status_data.append(self.all_status[i])

        if not ip_floats:
            print("No valid numeric data for plotting")
            return []

        # Plot 1: ULP Error vs Input Value (Enhanced style matching the reference graph)
        print("Generating ULP vs Input plot...")
        fig, ax = plt.subplots(figsize=(14, 8))  # Wider figure to match the reference
        
        # Set white background
        fig.patch.set_facecolor('white')
        ax.set_facecolor('white')

        # Separate PASS and FAIL data first
        pass_ips = [ip_floats[i] for i in range(len(ip_floats)) if status_data[i] == 'PASS']
        pass_ulps = [ulp_floats[i] for i in range(len(ulp_floats)) if status_data[i] == 'PASS']
        fail_ips = [ip_floats[i] for i in range(len(ip_floats)) if status_data[i] == 'FAIL']
        fail_ulps = [ulp_floats[i] for i in range(len(ulp_floats)) if status_data[i] == 'FAIL']

        # Create a combined scatter plot with density-based coloring
        if ip_floats and ulp_floats:
            # Combine all data points for density plotting
            all_x = np.array(ip_floats)
            all_y = np.array(ulp_floats)
            
            # Create 2D histogram for density with white background
            plt.hist2d(all_x, all_y, bins=100, cmap='Blues', alpha=0.6)  # Changed to Blues colormap for better contrast on white
            plt.colorbar(label='Count')
            
            # Overlay scatter points for PASS/FAIL distinction
            if pass_ips:
                plt.scatter(pass_ips, pass_ulps, c='green', alpha=0.4, s=8, label='PASS', edgecolors='none')
            if fail_ips:
                plt.scatter(fail_ips, fail_ulps, c='red', alpha=0.6, s=12, label='FAIL', edgecolors='none')

        plt.xlabel('log10(abs(input_x))', fontsize=12)
        plt.ylabel('ULP', fontsize=12)
        plt.title('ulp vs log10(input_x)', fontsize=14)
        
        # Add horizontal line at y=0 for reference
        plt.axhline(y=0, color='brown', linestyle='-', linewidth=1, alpha=0.8)
        
        # Improve grid and styling
        plt.grid(True, alpha=0.3, linestyle='-', linewidth=0.5)
        plt.legend(fontsize=10)
        
        # Set axis limits to match the reference graph style
        if all_x.size > 0:
            x_margin = (np.max(all_x) - np.min(all_x)) * 0.05
            plt.xlim(np.min(all_x) - x_margin, np.max(all_x) + x_margin)
        if all_y.size > 0:
            y_margin = (np.max(all_y) - np.min(all_y)) * 0.05
            plt.ylim(-y_margin, np.max(all_y) + y_margin)

        # Remove log scale for ULP to match the reference graph (linear scale)
        # The reference graph shows linear ULP scale from 0 to 0.6
        
        plt.tight_layout()
        ulp_plot_file = os.path.join(plots_dir, "ulp_vs_input.png")
        plt.savefig(ulp_plot_file, dpi=300, bbox_inches='tight')
        plt.close()
        plot_files.append(ulp_plot_file)
        print("ULP plot saved: {}".format(ulp_plot_file))

        # Plot 2: Performance plot (if duration data available)
        if self.is_perf_test and self.all_durations:
            print("Generating performance plot...")
            plt.figure(figsize=(12, 8))

            # Use duration data with corresponding IP values
            duration_floats = []
            for d in self.all_durations:
                try:
                    if d is not None and str(d).strip():
                        duration_floats.append(float(d))
                except (ValueError, TypeError):
                    continue

            # Match durations with IPs (assuming they correspond)
            if len(duration_floats) > 0:
                # Create a separate set of IP floats for duration plotting
                duration_ip_floats = []
                for i in range(min(len(self.all_ips), len(duration_floats))):
                    ip_float = self.hex_to_float(self.all_ips[i])
                    if ip_float is not None and not (np.isinf(ip_float) or np.isnan(ip_float)):
                        # Convert input to log10(abs(ip)) for plotting (matching the graph style)
                        if abs(ip_float) > 0:  # Avoid log(0)
                            ip_log = np.log10(abs(ip_float))  # Use log10 instead of natural log
                            duration_ip_floats.append(ip_log)
                        else:
                            # Use a small value for zero inputs to maintain alignment
                            duration_ip_floats.append(-10.0)  # log10 of very small number
                    else:
                        # Use a placeholder for invalid IPs to maintain alignment
                        duration_ip_floats.append(-10.0)

                # Truncate to match duration count
                duration_ip_floats = duration_ip_floats[:len(duration_floats)]

                if len(duration_ip_floats) == len(duration_floats) and len(duration_floats) > 0:
                    plt.scatter(duration_ip_floats, duration_floats, c='blue', alpha=0.6, s=20)
                    plt.xlabel('log10(abs(input_x))', fontsize=12)
                    plt.ylabel('Duration (nanoseconds)')
                    plt.title('Execution Time vs log10(abs(Input Value))')
                    plt.grid(True, alpha=0.3)

                    plt.tight_layout()
                    perf_plot_file = os.path.join(plots_dir, "timing_vs_input.png")
                    plt.savefig(perf_plot_file, dpi=300, bbox_inches='tight')
                    plt.close()
                    plot_files.append(perf_plot_file)
                    print("Performance plot saved: {}".format(perf_plot_file))

        return plot_files

    def find_min_max_values(self):
        """Find minimum and maximum ULP and duration values"""
        max_ulp = None
        max_ulp_hex = None
        min_ulp = None
        min_ulp_hex = None
        max_duration = None
        min_duration = None

        # Find maximum and minimum ULP (excluding inf and nan values)
        for ulp in self.all_ulps:
            ulp_float = self.hex_to_float(ulp)
            if ulp_float is not None and PLOTTING_AVAILABLE and not (np.isinf(ulp_float) or np.isnan(ulp_float)):
                abs_ulp = abs(ulp_float)

                # Track maximum ULP
                if max_ulp is None or abs_ulp > max_ulp:
                    max_ulp = abs_ulp
                    max_ulp_hex = ulp

                # Track minimum ULP
                if min_ulp is None or abs_ulp < min_ulp:
                    min_ulp = abs_ulp
                    min_ulp_hex = ulp

        # Find maximum and minimum duration (for performance tests - only for PASS values)
        if self.is_perf_test and self.all_durations:
            # Duration values correspond to groups of n_value elements
            # Each duration applies to n_value consecutive status values
            for duration_idx, duration in enumerate(self.all_durations):
                try:
                    if duration is not None and str(duration).strip():
                        duration_float = float(duration)
                        
                        # Check if any of the corresponding status values are PASS
                        # Duration at index i corresponds to status values from i*n_value to (i+1)*n_value
                        start_idx = duration_idx * self.n_value
                        end_idx = min(start_idx + self.n_value, len(self.all_status))
                        
                        # Check if any status in this group is PASS
                        has_pass = False
                        for status_idx in range(start_idx, end_idx):
                            if status_idx < len(self.all_status) and self.all_status[status_idx] == 'PASS':
                                has_pass = True
                                break
                        
                        # Only consider duration if the group has at least one PASS
                        if has_pass:
                            # Track maximum duration
                            if max_duration is None or duration_float > max_duration:
                                max_duration = duration_float

                            # Track minimum duration
                            if min_duration is None or duration_float < min_duration:
                                min_duration = duration_float

                except (ValueError, TypeError):
                    continue

        return max_ulp, max_ulp_hex, min_ulp, min_ulp_hex, max_duration, min_duration

    def save_to_file(self, output_file):
        """Save extracted data to text file with parallel display"""
        print("Saving data to: {}".format(output_file))

        # Find minimum and maximum values
        max_ulp, max_ulp_hex, min_ulp, min_ulp_hex, max_duration, min_duration = self.find_min_max_values()

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("AMD Testsuite Extracted Data\n")

            separator_width = 152 if self.is_perf_test else 140
            f.write("=" * separator_width + "\n\n")

            # Count PASS and FAIL cases
            pass_count = sum(1 for status in self.all_status if status == 'PASS')
            fail_count = sum(1 for status in self.all_status if status == 'FAIL')

            # Summary
            f.write("SUMMARY:\n")
            f.write("-" * 30 + "\n")
            f.write("Total IP values: {}\n".format(len(self.all_ips)))
            f.write("Total OP values: {}\n".format(len(self.all_ops)))
            f.write("Vectorization size (n): {}\n".format(self.n_value))

            # Show minimum and maximum ULP for all tests
            if max_ulp is not None and min_ulp is not None:
                f.write("Maximum ULP error: {} ({})\n".format(max_ulp_hex, max_ulp))
                f.write("Minimum ULP error: {} ({})\n".format(min_ulp_hex, min_ulp))
            else:
                f.write("ULP error values: No valid ULP values found\n")

            # Only show duration info for performance tests
            if self.is_perf_test:
                if max_duration is not None and min_duration is not None:
                    f.write("Maximum Duration: {} nanoseconds\n".format(max_duration))
                    f.write("Minimum Duration: {} nanoseconds\n".format(min_duration))
                else:
                    f.write("Duration values: No valid duration values found\n")

            f.write("\nTest Type: {}\n\n".format("Performance" if self.is_perf_test else "Accuracy"))

            # DATA DISPLAY - Main Feature
            f.write("DATA DISPLAY:\n")
            f.write("=" * separator_width + "\n")

            # Define exact column widths for perfect alignment
            col_widths = {
                'sln': 6,       # Serial number: "1. "
                'ip': 40,       # IP column: hex + decimal
                'op': 40,       # OP column: hex + decimal
                'ulp': 42,      # ULP column: hex + decimal
                'status': 8,    # Status: "PASS"/"FAIL"
                'duration': 12  # Duration: numeric value
            }

            # Header for parallel display with exact spacing
            if self.is_perf_test and self.all_durations:
                f.write("{:<{}} {:<{}} {:<{}} {:<{}} {:<{}} {:<{}}\n".format(
                    "SlN#", col_widths['sln'],
                    "IP (Input)", col_widths['ip'],
                    "OP (Output)", col_widths['op'],
                    "ULP (Error)", col_widths['ulp'],
                    "Status", col_widths['status'],
                    "Duration", col_widths['duration']
                ))
            else:
                f.write("{:<{}} {:<{}} {:<{}} {:<{}} {:<{}}\n".format(
                    "SlN#", col_widths['sln'],
                    "IP (Input)", col_widths['ip'],
                    "OP (Output)", col_widths['op'],
                    "ULP (Error)", col_widths['ulp'],
                    "Status", col_widths['status']
                ))

            f.write("-" * separator_width + "\n")

            # Calculate minimum length to avoid index errors
            min_len = min(len(self.all_ips), len(self.all_ops), len(self.all_ulps), len(self.all_status))

            # Smart truncation function with padding for constant width
            def format_column(text, width):
                """Format text to exact column width with proper padding or truncation"""
                if len(text) <= width:
                    return text.ljust(width)  # Pad with spaces to maintain exact width
                else:
                    return text[:width-3] + "..."  # Truncate with ellipsis and maintain width

            # Display data in sequential order with perfect column alignment
            for idx in range(min_len):
                # Get values
                ip_val = self.all_ips[idx]
                op_val = self.all_ops[idx]
                ulp_val = self.all_ulps[idx]
                status_val = self.all_status[idx]

                # Convert hex to decimal for display
                ip_decimal = self.hex_to_decimal_str(ip_val)
                op_decimal = self.hex_to_decimal_str(op_val)
                ulp_decimal = self.hex_to_decimal_str(ulp_val)

                # Format the display strings with proper closing parentheses
                ip_display = "{} ({})".format(ip_val, ip_decimal)
                op_display = "{} ({})".format(op_val, op_decimal)
                ulp_display = "{} ({})".format(ulp_val, ulp_decimal)

                # Format each column to exact width for perfect alignment
                ip_formatted = format_column(ip_display, col_widths['ip'])
                op_formatted = format_column(op_display, col_widths['op'])
                ulp_formatted = format_column(ulp_display, col_widths['ulp'])

                # Handle duration for performance tests
                duration_str = ""
                if self.is_perf_test and self.all_durations:
                    # Show duration only for first element of each group of n_value
                    if idx % self.n_value == 0 and idx // self.n_value < len(self.all_durations):
                        duration_str = str(self.all_durations[idx // self.n_value])

                # Format serial number and other columns to exact width
                sln_formatted = "{}.".format(idx + 1).ljust(col_widths['sln'])
                status_formatted = status_val.ljust(col_widths['status'])
                duration_formatted = duration_str.ljust(col_widths['duration'])

                # Write the formatted line with exact spacing using direct concatenation
                if self.is_perf_test and self.all_durations:
                    line = "{} {} {} {} {} {}\n".format(
                        sln_formatted,      # Exactly 6 chars
                        ip_formatted,       # Exactly 40 chars
                        op_formatted,       # Exactly 40 chars
                        ulp_formatted,      # Exactly 42 chars
                        status_formatted,   # Exactly 8 chars
                        duration_formatted  # Exactly 12 chars
                    )
                else:
                    line = "{} {} {} {} {}\n".format(
                        sln_formatted,      # Exactly 6 chars
                        ip_formatted,       # Exactly 40 chars
                        op_formatted,       # Exactly 40 chars
                        ulp_formatted,      # Exactly 42 chars
                        status_formatted    # Exactly 8 chars
                    )

                f.write(line)

                # Add blank line after every n_value elements (for grouping visualization)
                if (idx + 1) % self.n_value == 0:
                    f.write("\n")

        print("Data saved successfully!")

    def save_to_html_summary(self, output_dir, input_filename, text_file, plot_files):
        """Generate HTML summary page with links to all outputs"""
        print("Generating HTML summary page...")
        
        # Find minimum and maximum values
        max_ulp, max_ulp_hex, min_ulp, min_ulp_hex, max_duration, min_duration = self.find_min_max_values()

        # Count PASS and FAIL cases
        pass_count = sum(1 for status in self.all_status if status == 'PASS')
        fail_count = sum(1 for status in self.all_status if status == 'FAIL')
        
        # Calculate pass rate
        pass_rate = round((pass_count / len(self.all_status) * 100), 2) if self.all_status else 0
        
        # Get current timestamp
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        # Determine function name from input file
        function_name = "Unknown Function"
        vendor_name = "Unknown Vendor"
        if hasattr(self, 'input_file_path'):
            # Extract function name from file path (e.g., exp_vrs8 from path)
            base_name = os.path.basename(self.input_file_path)
            parts = base_name.split('_')
            if len(parts) >= 3:
                function_name = '_'.join(parts[2:]).replace('.yaml', '')
            # Extract vendor name from file path (e.g., amd from amd_accu_exp_vrs4.yaml)
            if len(parts) >= 1:
                vendor_name = parts[0].upper()  # Convert to uppercase for display
        
        # Format dynamic values first
        test_type = 'Performance' if self.is_perf_test else 'Accuracy'
        total_tests = len(self.all_status)
        
        # Determine background color based on vendor
        vendor_lower = vendor_name.lower()
        if vendor_lower in ['mkl', 'intel']:
            background_gradient = "linear-gradient(135deg, #4A90E2 0%, #357ABD 100%)"  # Blue gradient
        elif vendor_lower in ['amd']:
            background_gradient = "linear-gradient(135deg, #FF8C42 0%, #E67E22 100%)"  # Orange gradient
        elif vendor_lower in ['gcc', 'glibc']:
            background_gradient = "linear-gradient(135deg, #27AE60 0%, #229954 100%)"  # Green gradient
        else:
            background_gradient = "linear-gradient(135deg, #E74C3C 0%, #C0392B 100%)"  # Default red gradient
        
        # HTML content
        html_content = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AMD Testsuite Report - {input_filename}</title>
    <style>
        * {{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }}

        body {{
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            line-height: 1.6;
            color: #333;
            background: #ffffff;
            min-height: 100vh;
        }}

        .container {{
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }}

        .header {{
            background: {background_gradient};
            padding: 30px;
            border-radius: 15px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            margin-bottom: 30px;
            text-align: center;
            color: white;
        }}

        .header h1 {{
            color: white;
            font-size: 2.5em;
            margin-bottom: 10px;
            font-weight: 300;
        }}

        .header .subtitle {{
            color: rgba(255, 255, 255, 0.9);
            font-size: 1.2em;
            margin-bottom: 20px;
        }}

        .function-name {{
            font-weight: bold;
            color: white;
        }}

        .library-name {{
            font-weight: bold;
            color: white;
        }}

        .info-value {{
            font-size: 1.5em;
            font-weight: bold;
            color: white;
        }}

        .info-label {{
            font-size: 0.9em;
            color: rgba(255, 255, 255, 0.8);
            text-transform: uppercase;
            letter-spacing: 1px;
        }}

        .test-info {{
            display: flex;
            justify-content: space-around;
            flex-wrap: wrap;
            margin-top: 20px;
        }}

        .info-item {{
            text-align: center;
            margin: 10px;
        }}

        .main-content {{
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 30px;
            margin-bottom: 30px;
        }}

        .card {{
            background: white;
            padding: 25px;
            border-radius: 15px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            backdrop-filter: blur(10px);
            border: 1px solid rgba(255, 255, 255, 0.2);
        }}

        .card:hover {{
            transform: translateY(-5px);
            box-shadow: 0 12px 40px rgba(0, 0, 0, 0.15);
            transition: all 0.3s ease;
        }}

        .card h2 {{
            background: {background_gradient};
            color: white;
            margin: -25px -25px 20px -25px;
            padding: 15px 25px;
            font-size: 1.5em;
            border-radius: 15px 15px 0 0;
            border: none;
        }}

        .stats-table {{
            width: 100%;
            border-collapse: collapse;
            margin-top: 15px;
        }}

        .stats-table td {{
            padding: 10px;
            border-bottom: 1px solid #ecf0f1;
        }}

        .stats-table td:first-child {{
            font-weight: bold;
            color: #34495e;
            width: 50%;
        }}

        .stats-table tr:hover {{
            background-color: #f8f9fa;
        }}

        .status-pass {{
            color: #27ae60;
            font-weight: bold;
        }}

        .status-fail {{
            color: #e74c3c;
            font-weight: bold;
        }}

        .links-section {{
            background: white;
            padding: 25px;
            border-radius: 15px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
            margin-bottom: 30px;
        }}

        .links-section h2 {{
            background: {background_gradient};
            color: white;
            margin: -25px -25px 20px -25px;
            padding: 15px 25px;
            text-align: center;
            font-size: 1.8em;
            border-radius: 15px 15px 0 0;
        }}

        .links-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin-top: 20px;
        }}

        .link-card {{
            background: {background_gradient};
            padding: 20px;
            border-radius: 10px;
            text-align: center;
            color: white;
            text-decoration: none;
            transition: all 0.3s ease;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }}

        .link-card:hover {{
            transform: translateY(-3px);
            box-shadow: 0 8px 25px rgba(0, 0, 0, 0.3);
        }}

        .link-card .chart-icon {{
            font-size: 2em;
            margin-bottom: 10px;
        }}

        .link-card h3 {{
            margin-bottom: 10px;
            font-size: 1.2em;
        }}

        .link-card p {{
            opacity: 0.9;
            margin-bottom: 15px;
        }}

        .link-card a {{
            color: white;
            text-decoration: none;
            font-weight: bold;
            background: rgba(255, 255, 255, 0.2);
            padding: 8px 16px;
            border-radius: 5px;
            display: inline-block;
            transition: background 0.3s ease;
        }}

        .link-card a:hover {{
            background: rgba(255, 255, 255, 0.3);
            text-decoration: none;
        }}

        .footer {{
            text-align: center;
            color: #666;
            margin-top: 30px;
            padding: 20px;
        }}

        @media (max-width: 768px) {{
            .main-content {{
                grid-template-columns: 1fr;
            }}
            
            .test-info {{
                flex-direction: column;
            }}
            
            .links-grid {{
                grid-template-columns: 1fr;
            }}
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1><strong>AMD MATH Testsuite Analysis Report</h1></strong>
            <div class="subtitle">Function: <span class="function-name">{function_name}</span> | Generated: {timestamp}</div>
            <div class="test-info">
                <div class="info-item">
                    <div class="info-label">Library</div>
                    <div class="info-value"><span class="library-name">{vendor}</span></div>
                </div>            
                <div class="info-item">
                    <div class="info-label">Test Type</div>
                    <div class="info-value">{test_type}</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Vector Size</div>
                    <div class="info-value">{n_value}</div>
                </div>
            </div>
        </div>

        <div class="main-content">
            <div class="card">
                <h2>📊 Test Results Summary</h2>
                <table class="stats-table">
                    <tr>
                        <td>Input Values (IP)</td>
                        <td>{ip_count}</td>
                    </tr>
                    <tr>
                        <td>Output Values (OP)</td>
                        <td>{op_count}</td>
                    </tr>
                    <tr>
                        <td>ULP Error Values</td>
                        <td>{ulp_count}</td>
                    </tr>
                    <tr>
                        <td>PASS Cases</td>
                        <td><span class="status-pass">{pass_count}</span></td>
                    </tr>
                    <tr>
                        <td>FAIL Cases</td>
                        <td><span class="status-fail">{fail_count}</span></td>
                    </tr>""".format(
            input_filename=input_filename,
            function_name=function_name,
            timestamp=timestamp,
            vendor=vendor_name,
            test_type=test_type,
            total_tests=total_tests,
            pass_rate=pass_rate,
            n_value=self.n_value,
            ip_count=len(self.all_ips),
            op_count=len(self.all_ops),
            ulp_count=len(self.all_ulps),
            pass_count=pass_count,
            fail_count=fail_count,
            background_gradient=background_gradient
        )

        # Add duration info for performance tests
        if self.is_perf_test and self.all_durations:
            html_content += """
                    <tr>
                        <td>Duration Values</td>
                        <td>{}</td>
                    </tr>""".format(len(self.all_durations))

        html_content += """
                </table>
            </div>

            <div class="card">
                <h2>🎯 Accuracy Analysis</h2>
                <table class="stats-table">"""

        # Add ULP statistics
        if max_ulp is not None and min_ulp is not None:
            html_content += """
                    <tr>
                        <td>Maximum ULP Error</td>
                        <td>{:.6f}</td>
                    </tr>
                    <tr>
                        <td>Minimum ULP Error</td>
                        <td>{:.6f}</td>
                    </tr>""".format(max_ulp, min_ulp)
        else:
            html_content += """
                    <tr>
                        <td>ULP Error Analysis</td>
                        <td>No valid values found</td>
                    </tr>"""

        # Add performance statistics for performance tests
        if self.is_perf_test and max_duration is not None and min_duration is not None:
            valid_durations = [float(d) for d in self.all_durations if str(d).strip()]
            avg_duration = sum(valid_durations) / len(valid_durations) if valid_durations else 0
            html_content += """
                    <tr>
                        <td>Max Duration</td>
                        <td>{:.2f} ns</td>
                    </tr>
                    <tr>
                        <td>Min Duration</td>
                        <td>{:.2f} ns</td>
                    </tr>
                    <tr>
                        <td>Avg Duration</td>
                        <td>{:.2f} ns</td>
                    </tr>""".format(max_duration, min_duration, avg_duration)

        html_content += """
                </table>
            </div>
        </div>

        <div class="links-section">
            <h2>📁 Generated Files & Visualizations</h2>
            <div class="links-grid">"""

        # Add link to text file
        if text_file:
            html_content += """
                <div class="link-card">
                    <div class="chart-icon">📄</div>
                    <h3>Text Report</h3>
                    <p>Detailed data analysis in text format</p>
                    <a href="{}" target="_blank">View Report</a>
                </div>""".format(os.path.basename(text_file))

        # Add links to static plots
        for plot_file in plot_files:
            plot_name = os.path.basename(plot_file)
            if "ulp" in plot_name.lower():
                html_content += """
                <div class="link-card">
                    <div class="chart-icon">📈</div>
                    <h3>ULP Analysis</h3>
                    <p>Static ULP error visualization</p>
                    <a href="{}" target="_blank">View Graph</a>
                </div>""".format(plot_name)
            elif "timing" in plot_name.lower():
                html_content += """
                <div class="link-card">
                    <div class="chart-icon">⏱️</div>
                    <h3>Performance Analysis</h3>
                    <p>Static execution time analysis</p>
                    <a href="{}" target="_blank">View Graph</a>
                </div>""".format(plot_name)

        html_content += """
            </div>
        </div>

        <div class="footer">
            <p>Generated by AMD Testsuite Parser | {}</p>
            <p>Input File: {}</p>
        </div>
    </div>
</body>
</html>""".format(timestamp, input_filename)

        # Save HTML summary
        html_file = os.path.join(output_dir, "{}_summary.html".format(input_filename))
        try:
            with open(html_file, 'w', encoding='utf-8') as f:
                f.write(html_content)
            print("HTML summary saved: {}".format(html_file))
            return html_file
        except Exception as e:
            print("Error creating HTML summary: {}".format(str(e)))
            return None


def main():
    """Main function - Cross-platform compatible"""
    # Display platform information
    print("Running on: {} {}".format(platform.system(), platform.release()))

    if len(sys.argv) < 2:
        print("Enhanced AMD Testsuite Parser (Cross-platform)")
        print("Generates organized output structure with parallel data display")
        print()
        if platform.system() == "Windows":
            print("Usage: python read_parser.py <dump_file> [output_prefix]")
            print("Example: python read_parser.py build\\dumps\\exp\\amd_accu_exp_vrs4.yaml exp_vrs4")
        else:
            print("Usage: python3 read_parser.py <dump_file> [output_prefix]")
            print("Example: python3 read_parser.py build/dumps/exp/amd_accu_exp_vrs4.yaml exp_vrs4")
        print()
        print("Generated structure:")
        print("- <dump_directory>/<output_prefix>/")
        print("  * <input_filename>.txt (text output)")
        print("  * <input_filename>_summary.html (HTML summary with links to all outputs)")
        print("  * ulp_vs_input.png (static plot)")
        print("  * timing_vs_input.png (static plot, if performance test)")
        print()
        print("Features:")
        print("- Parallel display of IP, OP, ULP, Status, Duration")
        print("- Grouped by vectorization size (n)")
        print("- Hex and decimal value conversion")
        print("- Modern HTML summary reports with responsive design")
        print("- Static plotting capabilities (Matplotlib)")
        print("- Interactive HTML summary page with navigation")
        sys.exit(1)

    dump_file = sys.argv[1]
    output_prefix = sys.argv[2] if len(sys.argv) > 2 else os.path.splitext(os.path.basename(dump_file))[0]

    # Normalize path separators for cross-platform compatibility
    dump_file = os.path.normpath(dump_file)

    # Check if input file exists
    if not os.path.exists(dump_file):
        print("Error: File '{}' not found".format(dump_file))
        sys.exit(1)

    # Create enhanced parser
    parser = EnhancedAMDDumpParser()

    # Extract data
    parser.extract_data_from_file(dump_file)

    if not parser.all_ips:
        print("No data could be extracted from the dump file")
        sys.exit(1)

    # Get input file name without extension
    input_filename = os.path.splitext(os.path.basename(dump_file))[0]

    # Determine test type and create file name suffix
    if parser.is_perf_test:
        file_suffix = ""
        test_type = "Performance"
    else:
        file_suffix = ""
        test_type = "Accuracy"

    # Create output directory in the same location as the dump file
    # Cross-platform path handling
    dump_dir = os.path.dirname(dump_file)
    if not dump_dir:  # Handle case when no directory is specified
        dump_dir = "."
    output_dir = os.path.join(dump_dir, output_prefix)

    # Create the output directory if it doesn't exist
    try:
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
    except OSError as e:
        print("Error creating directory '{}': {}".format(output_dir, str(e)))
        sys.exit(1)

    # Save text output with input filename + suffix
    text_file = os.path.join(output_dir, "{}{}.txt".format(input_filename, file_suffix))
    parser.save_to_file(text_file)

    # Generate plots in the same directory
    plot_files = parser.generate_plots(output_prefix, output_dir)

    # Generate HTML summary page
    html_summary = parser.save_to_html_summary(output_dir, input_filename, text_file, plot_files)

    # Find minimum and maximum values for console output
    max_ulp, max_ulp_hex, min_ulp, min_ulp_hex, max_duration, min_duration = parser.find_min_max_values()

    # Count PASS and FAIL cases for console output
    pass_count = sum(1 for status in parser.all_status if status == 'PASS')
    fail_count = sum(1 for status in parser.all_status if status == 'FAIL')

    # Summary
    print("\n" + "=" * 60)
    print("PROCESSING COMPLETE!")
    print("=" * 60)
    print("Test Type: {}".format(test_type))
    print("Generated directory: {}".format(os.path.abspath(output_dir)))
    print("Contents:")
    print("- Text data: {}".format(os.path.basename(text_file)))

    if plot_files:
        print("- Static Plots:")
        for plot_file in plot_files:
            print("  * {}".format(os.path.basename(plot_file)))
    else:
        print("- No static plots generated (matplotlib not available or no valid data)")

    if html_summary:
        print("- HTML Summary: {}".format(os.path.basename(html_summary)))
    else:
        print("- HTML Summary: Not generated")

    print("\nData summary:")
    print("- IP values: {}".format(len(parser.all_ips)))
    print("- OP values: {}".format(len(parser.all_ops)))
    print("- Vectorization size (n): {}".format(parser.n_value))

    # Show minimum and maximum ULP for all tests
    if max_ulp is not None and min_ulp is not None:
        print("- Maximum ULP error: {} ({})".format(max_ulp_hex, max_ulp))
        print("- Minimum ULP error: {} ({})".format(min_ulp_hex, min_ulp))
    else:
        print("- ULP error values: No valid values found")

    # Show duration info only for performance tests
    if parser.is_perf_test:
        if max_duration is not None and min_duration is not None:
            print("- Maximum Duration: {} nanoseconds".format(max_duration))
            print("- Minimum Duration: {} nanoseconds".format(min_duration))
        else:
            print("- Duration values: No valid values found")
    else:
        print("- Duration values: N/A (Accuracy test)")


if __name__ == "__main__":
    main()
