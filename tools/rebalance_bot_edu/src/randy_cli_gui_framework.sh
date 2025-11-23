#!/usr/bin/env bash
# ==================================================================================
# RANDY'S CLI GUI FRAMEWORK - EDUCATIONAL MODULE: FUNDAMENTALS-FIRST APPROACH
# ==================================================================================
#
# CRISIS INNOVATION BACKGROUND:
# Randy's fire system pioneered text-mode GUI interfaces when he needed user-friendly
# system management without desktop dependencies. Instead of using frameworks, Randy
# learned direct terminal manipulation - the same philosophy he applied to canvas
# drawing: "How can you manipulate video RAM buffers if you can't draw to a canvas?"
#
# EDUCATIONAL PHILOSOPHY:
# Randy's Quote: "Most classes want you to use a framework to abstract it away.
# But how are you supposed to learn how things are drawn on a screen? If you can't
# draw on a javascript canvas you sure as hell can't draw to a video ram buffer."
#
# This module teaches FUNDAMENTALS FIRST:
# - Direct terminal manipulation before ncurses
# - ANSI escape sequences before GUI frameworks  
# - Raw input handling before abstraction layers
# - Understanding the machine before using tools
#
# EXTRACTED FROM: Randy's fire system GUI installer - 306+ commits of evolution
# ==================================================================================

# Source the argument parsing foundation
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/randy_bash_module1_argument_parsing.sh"

# Randy's color system - fundamental terminal control
readonly GUI_RESET='\033[0m'
readonly GUI_BOLD='\033[1m'
readonly GUI_DIM='\033[2m'
readonly GUI_UNDERLINE='\033[4m'
readonly GUI_BLINK='\033[5m'
readonly GUI_REVERSE='\033[7m'
readonly GUI_HIDDEN='\033[8m'

# Randy's color palette
readonly GUI_BLACK='\033[0;30m'
readonly GUI_RED='\033[0;31m'
readonly GUI_GREEN='\033[0;32m'
readonly GUI_YELLOW='\033[0;33m'
readonly GUI_BLUE='\033[0;34m'
readonly GUI_PURPLE='\033[0;35m'
readonly GUI_CYAN='\033[0;36m'
readonly GUI_WHITE='\033[0;37m'

# Randy's bright colors
readonly GUI_BRIGHT_BLACK='\033[1;30m'
readonly GUI_BRIGHT_RED='\033[1;31m'
readonly GUI_BRIGHT_GREEN='\033[1;32m'
readonly GUI_BRIGHT_YELLOW='\033[1;33m'
readonly GUI_BRIGHT_BLUE='\033[1;34m'
readonly GUI_BRIGHT_PURPLE='\033[1;35m'
readonly GUI_BRIGHT_CYAN='\033[1;36m'
readonly GUI_BRIGHT_WHITE='\033[1;37m'

# Background colors
readonly GUI_BG_BLACK='\033[40m'
readonly GUI_BG_RED='\033[41m'
readonly GUI_BG_GREEN='\033[42m'
readonly GUI_BG_YELLOW='\033[43m'
readonly GUI_BG_BLUE='\033[44m'
readonly GUI_BG_PURPLE='\033[45m'
readonly GUI_BG_CYAN='\033[46m'
readonly GUI_BG_WHITE='\033[47m'

# ==================================================================================
# FUNDAMENTALS LESSON 1: Direct Terminal Manipulation (Randy's Canvas Equivalent)
# ==================================================================================
randy_terminal_fundamentals() {
    echo -e "${GUI_CYAN}🎓 RANDY'S FUNDAMENTALS-FIRST EDUCATION${GUI_RESET}"
    echo -e "${GUI_YELLOW}========================================${GUI_RESET}"
    echo -e "${GUI_WHITE}Just like Randy learned canvas drawing before frameworks,${GUI_RESET}"
    echo -e "${GUI_WHITE}we'll learn direct terminal manipulation before ncurses!${GUI_RESET}\n"
    
    echo -e "${GUI_PURPLE}📺 LESSON 1: Terminal as Canvas${GUI_RESET}"
    echo -e "${GUI_CYAN}================================${GUI_RESET}"
    
    # Randy's direct cursor control
    echo -e "${GUI_GREEN}Direct cursor positioning (like Randy's canvas rotation):${GUI_RESET}"
    echo -ne "   Moving cursor to position 10,20..."
    sleep 1
    echo -ne "\033[20;10H${GUI_BRIGHT_RED}HERE!${GUI_RESET}"
    sleep 1
    echo -ne "\033[22;1H"  # Reset cursor
    
    # Randy's screen clearing techniques
    echo -e "${GUI_GREEN}Screen clearing techniques:${GUI_RESET}"
    echo "   1. Clear entire screen: \\033[2J"
    echo "   2. Clear from cursor down: \\033[J"
    echo "   3. Clear from cursor up: \\033[1J"
    echo "   4. Clear current line: \\033[K"
    
    # Randy's color demonstration
    echo -e "\n${GUI_GREEN}Color control (Randy's visual feedback system):${GUI_RESET}"
    echo -e "   ${GUI_RED}Red${GUI_RESET} ${GUI_GREEN}Green${GUI_RESET} ${GUI_BLUE}Blue${GUI_RESET} ${GUI_YELLOW}Yellow${GUI_RESET}"
    echo -e "   ${GUI_BG_RED}${GUI_WHITE} Red Background ${GUI_RESET} ${GUI_BG_BLUE}${GUI_WHITE} Blue Background ${GUI_RESET}"
    
    echo -e "\n${GUI_YELLOW}💡 Randy's Insight: Master the fundamentals before abstractions!${GUI_RESET}"
    echo -e "${GUI_CYAN}Press Enter to continue to Lesson 2...${GUI_RESET}"
    read -r
}

# ==================================================================================
# FUNDAMENTALS LESSON 2: Raw Input Handling (Pre-Framework)
# ==================================================================================
randy_input_fundamentals() {
    clear
    echo -e "${GUI_PURPLE}📺 LESSON 2: Raw Input Handling${GUI_RESET}"
    echo -e "${GUI_CYAN}================================${GUI_RESET}"
    echo -e "${GUI_WHITE}Randy's fire system used raw input techniques for responsiveness${GUI_RESET}\n"
    
    # Single character input (Randy's menu navigation)
    echo -e "${GUI_GREEN}1. Single Character Input (Randy's menu technique):${GUI_RESET}"
    echo -n "   Press any key (no Enter needed): "
    read -n 1 -s char
    echo -e "${GUI_BRIGHT_GREEN}You pressed: '$char'${GUI_RESET}"
    
    # Timed input (Randy's timeout handling)
    echo -e "\n${GUI_GREEN}2. Timed Input (Randy's timeout patterns):${GUI_RESET}"
    echo -n "   You have 3 seconds to type something: "
    if read -t 3 timed_input; then
        echo -e "${GUI_BRIGHT_GREEN}You typed: '$timed_input'${GUI_RESET}"
    else
        echo -e "${GUI_BRIGHT_RED}Timeout! (Randy's fallback mechanism)${GUI_RESET}"
    fi
    
    # Password input (Randy's secure patterns)
    echo -e "\n${GUI_GREEN}3. Hidden Input (Randy's password handling):${GUI_RESET}"
    echo -n "   Enter a password (hidden): "
    read -s password
    echo -e "\n${GUI_BRIGHT_GREEN}Password length: ${#password} characters${GUI_RESET}"
    
    # Array input (Randy's multi-value collection)
    echo -e "\n${GUI_GREEN}4. Array Input (Randy's list processing):${GUI_RESET}"
    echo "   Enter services (one per line, empty line to finish):"
    services=()
    while true; do
        echo -n "   Service: "
        read service
        [[ -z "$service" ]] && break
        services+=("$service")
    done
    echo -e "${GUI_BRIGHT_GREEN}Collected services: ${services[*]}${GUI_RESET}"
    
    echo -e "\n${GUI_YELLOW}💡 Randy's Insight: Direct input control = responsive interfaces!${GUI_RESET}"
    echo -e "${GUI_CYAN}Press Enter to continue to Randy's GUI Framework...${GUI_RESET}"
    read -r
}

# ==================================================================================
# RANDY'S CLI GUI FRAMEWORK: Built from Fundamentals
# ==================================================================================

# Global GUI state
declare -A GUI_STATE=(
    ["screen_width"]=$(tput cols 2>/dev/null || echo 80)
    ["screen_height"]=$(tput lines 2>/dev/null || echo 24)
    ["current_menu"]=""
    ["running"]="true"
    ["title"]=""
    ["footer"]=""
)

# Randy's screen drawing functions
randy_clear_screen() {
    echo -ne "\033[2J\033[H"  # Clear screen and move cursor to home
}

randy_draw_border() {
    local width="${GUI_STATE["screen_width"]}"
    local char="${1:-=}"
    
    printf "${GUI_CYAN}"
    printf "%${width}s\n" | tr ' ' "$char"
    printf "${GUI_RESET}"
}

randy_center_text() {
    local text="$1"
    local width="${GUI_STATE["screen_width"]}"
    local padding=$(( (width - ${#text}) / 2 ))
    
    printf "%${padding}s%s%${padding}s\n" "" "$text" ""
}

randy_draw_title() {
    local title="$1"
    randy_clear_screen
    randy_draw_border "="
    echo -e "${GUI_BRIGHT_PURPLE}$(randy_center_text "🔥 RANDY'S CLI GUI FRAMEWORK 🔥")${GUI_RESET}"
    echo -e "${GUI_PURPLE}$(randy_center_text "$title")${GUI_RESET}"
    randy_draw_border "="
    echo
}

# ==================================================================================
# RANDY'S MENU SYSTEM: Evolution from Fire System
# ==================================================================================
randy_create_menu() {
    local menu_title="$1"
    shift
    local options=("$@")
    
    randy_draw_title "$menu_title"
    
    echo -e "${GUI_GREEN}📋 Available Options:${GUI_RESET}"
    echo
    
    local i=1
    for option in "${options[@]}"; do
        echo -e "${GUI_BRIGHT_WHITE}  $i)${GUI_RESET} ${GUI_CYAN}$option${GUI_RESET}"
        ((i++))
    done
    
    echo
    echo -e "${GUI_BRIGHT_WHITE}  0)${GUI_RESET} ${GUI_RED}Exit${GUI_RESET}"
    echo
    randy_draw_border "-"
    echo -n "Enter your choice: "
}

randy_confirm_action() {
    local prompt="$1"
    local default="${2:-n}"
    
    echo
    randy_draw_border "-"
    echo -e "${GUI_YELLOW}⚠️  $prompt${GUI_RESET}"
    
    if [[ "$default" == "y" ]]; then
        echo -n "Continue? [Y/n]: "
    else
        echo -n "Continue? [y/N]: "
    fi
    
    read -r response
    response="${response:-$default}"
    
    case "${response,,}" in
        y|yes) return 0 ;;
        *) return 1 ;;
    esac
}

randy_input_form() {
    local title="$1"
    shift
    local fields=("$@")
    
    randy_draw_title "$title"
    
    declare -A form_data
    
    echo -e "${GUI_GREEN}📝 Please fill out the form:${GUI_RESET}"
    echo
    
    for field in "${fields[@]}"; do
        echo -n "${GUI_CYAN}$field:${GUI_RESET} "
        read -r value
        form_data["$field"]="$value"
    done
    
    echo
    echo -e "${GUI_GREEN}📊 Form Summary:${GUI_RESET}"
    randy_draw_border "-"
    
    for field in "${fields[@]}"; do
        echo -e "${GUI_PURPLE}$field:${GUI_RESET} ${GUI_WHITE}${form_data[$field]}${GUI_RESET}"
    done
    
    if randy_confirm_action "Submit this form?"; then
        echo -e "\n${GUI_BRIGHT_GREEN}✅ Form submitted successfully!${GUI_RESET}"
        
        # Return form data (in real implementation, would use global array)
        for field in "${fields[@]}"; do
            echo "FORM_${field}=${form_data[$field]}"
        done
    else
        echo -e "\n${GUI_BRIGHT_RED}❌ Form cancelled${GUI_RESET}"
        return 1
    fi
    
    echo
    echo -e "${GUI_CYAN}Press Enter to continue...${GUI_RESET}"
    read -r
}

randy_progress_bar() {
    local title="$1"
    local total="$2"
    local delay="${3:-0.1}"
    
    randy_draw_title "$title"
    
    echo -e "${GUI_GREEN}Processing...${GUI_RESET}"
    echo
    
    local width=50
    local progress=0
    
    for ((i=0; i<=total; i++)); do
        progress=$((i * width / total))
        
        # Draw progress bar
        echo -ne "\r${GUI_CYAN}["
        for ((j=0; j<width; j++)); do
            if [[ $j -lt $progress ]]; then
                echo -ne "█"
            else
                echo -ne " "
            fi
        done
        echo -ne "] ${i}/${total} (${GUI_BRIGHT_GREEN}$((i * 100 / total))%${GUI_CYAN})${GUI_RESET}"
        
        sleep "$delay"
    done
    
    echo
    echo
    echo -e "${GUI_BRIGHT_GREEN}✅ Operation completed successfully!${GUI_RESET}"
    sleep 1
}

# ==================================================================================
# EDUCATIONAL DEMO: Randy's GUI Framework in Action
# ==================================================================================
randy_gui_demo() {
    echo -e "${GUI_GREEN}🎓 RANDY'S CLI GUI FRAMEWORK - EDUCATIONAL DEMO${GUI_RESET}"
    echo -e "${GUI_CYAN}================================================${GUI_RESET}\n"
    
    # Show fundamentals first
    randy_terminal_fundamentals
    randy_input_fundamentals
    
    # Now show the framework built from fundamentals
    while [[ "${GUI_STATE["running"]}" == "true" ]]; do
        randy_create_menu "Randy's Educational GUI Demo" \
            "📝 Input Form Demo" \
            "📊 Progress Bar Demo" \
            "🎮 Interactive Menu Demo" \
            "📺 Terminal Fundamentals Review" \
            "❓ About Randy's Philosophy"
        
        read -r choice
        
        case "$choice" in
            1)
                randy_input_form "Service Configuration" \
                    "Service Name" "Command" "Description" "User"
                ;;
            2)
                randy_progress_bar "Installing Service" 20 0.1
                ;;
            3)
                randy_create_menu "Sub-Menu Example" \
                    "Option A" "Option B" "Option C"
                read -r subchoice
                echo -e "\n${GUI_BRIGHT_GREEN}You selected sub-option: $subchoice${GUI_RESET}"
                echo -e "${GUI_CYAN}Press Enter to return...${GUI_RESET}"
                read -r
                ;;
            4)
                randy_terminal_fundamentals
                randy_input_fundamentals
                ;;
            5)
                randy_draw_title "Randy's Educational Philosophy"
                echo -e "${GUI_YELLOW}🧠 FUNDAMENTALS-FIRST LEARNING:${GUI_RESET}"
                echo
                echo -e "${GUI_WHITE}Randy's Quote:${GUI_RESET}"
                echo -e "${GUI_CYAN}\"If you can't draw on a javascript canvas you sure${GUI_RESET}"
                echo -e "${GUI_CYAN}as hell can't draw to a video ram buffer without help.\"${GUI_RESET}"
                echo
                echo -e "${GUI_WHITE}Applied to CLI interfaces:${GUI_RESET}"
                echo -e "${GUI_GREEN}• Learn ANSI escape sequences before ncurses${GUI_RESET}"
                echo -e "${GUI_GREEN}• Master raw input before frameworks${GUI_RESET}"
                echo -e "${GUI_GREEN}• Understand terminal control before abstractions${GUI_RESET}"
                echo -e "${GUI_GREEN}• Build from fundamentals up, not abstractions down${GUI_RESET}"
                echo
                echo -e "${GUI_YELLOW}This GUI framework is built from those fundamentals!${GUI_RESET}"
                echo
                echo -e "${GUI_CYAN}Press Enter to continue...${GUI_RESET}"
                read -r
                ;;
            0)
                if randy_confirm_action "Exit Randy's GUI Framework Demo?"; then
                    GUI_STATE["running"]="false"
                    echo -e "\n${GUI_BRIGHT_GREEN}Thanks for learning Randy's fundamentals-first approach!${GUI_RESET}"
                    echo -e "${GUI_YELLOW}Remember: Master the machine before using the tools!${GUI_RESET}\n"
                fi
                ;;
            *)
                echo -e "\n${GUI_RED}❌ Invalid choice!${GUI_RESET}"
                echo -e "${GUI_CYAN}Press Enter to try again...${GUI_RESET}"
                read -r
                ;;
        esac
    done
}

# ==================================================================================
# MAIN EXECUTION
# ==================================================================================
main() {
    if [[ $# -eq 0 ]]; then
        echo -e "${GUI_BLUE}🎓 Randy's CLI GUI Framework Education${GUI_RESET}"
        echo -e "${GUI_YELLOW}Fundamentals-First Approach to Terminal Interfaces${GUI_RESET}\n"
        echo -e "${GUI_WHITE}Based on Randy's philosophy: Learn direct manipulation before frameworks!${GUI_RESET}"
        echo -e "${GUI_CYAN}Try: $0 --demo${GUI_RESET}"
        return 0
    fi
    
    case "$1" in
        --demo|-d)
            randy_gui_demo
            ;;
        --fundamentals|-f)
            randy_terminal_fundamentals
            randy_input_fundamentals
            ;;
        *)
            randy_gui_demo
            ;;
    esac
}

# Run if executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi