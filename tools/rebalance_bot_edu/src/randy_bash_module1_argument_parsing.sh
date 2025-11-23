#!/usr/bin/env bash
# ==================================================================================
# RANDY'S ADVANCED BASH CONCEPTS - EDUCATIONAL MODULE 1: ARGUMENT PARSING MASTERY
# ==================================================================================
#
# CRISIS INNOVATION BACKGROUND:
# This module extracts Randy's advanced argument parsing techniques developed 
# through 306+ commits of the "fire" system evolution. Randy learned these 
# patterns under pressure when building production service management tools.
#
# EDUCATIONAL VALUE:
# - Advanced getopt usage and parameter validation
# - Flexible argument processing with default handling
# - Error checking and user feedback patterns
# - Configuration merging and precedence rules
# - Real-world production argument parsing techniques
#
# EXTRACTED FROM: Randy's fire system - universal_service_manager.sh
# ==================================================================================

# Global configuration - Randy's approach to default values
declare -A RANDY_CONFIG=(
    ["SERVICE_NAME"]=""
    ["OPERATION"]=""
    ["CONFIG_FILE"]=""
    ["LOG_LEVEL"]="INFO"
    ["DRY_RUN"]="false"
    ["FORCE"]="false"
    ["QUIET"]="false"
    ["VERBOSE"]="false"
    ["AUTO_CONFIRM"]="false"
    ["BACKUP_ENABLED"]="true"
)

# Randy's color system for educational feedback
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly PURPLE='\033[0;35m'
readonly CYAN='\033[0;36m'
readonly WHITE='\033[1;37m'
readonly NC='\033[0m' # No Color

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Advanced Argument Parsing Pattern
# ==================================================================================
parse_randy_arguments() {
    local TEMP
    
    # Randy's getopt pattern - handles both short and long options
    TEMP=$(getopt -o 's:o:c:l:dfqvyb' \
                  --long 'service:,operation:,config:,log-level:,dry-run,force,quiet,verbose,yes,backup:' \
                  -n 'randy_bash_education' -- "$@")
    
    if [[ $? -ne 0 ]]; then
        echo -e "${RED}❌ ERROR: Invalid arguments provided${NC}" >&2
        show_randy_usage
        return 1
    fi
    
    eval set -- "$TEMP"
    unset TEMP
    
    # Randy's argument processing loop
    while true; do
        case "$1" in
            '-s'|'--service')
                RANDY_CONFIG["SERVICE_NAME"]="$2"
                shift 2
                continue
                ;;
            '-o'|'--operation')
                RANDY_CONFIG["OPERATION"]="$2"
                shift 2
                continue
                ;;
            '-c'|'--config')
                RANDY_CONFIG["CONFIG_FILE"]="$2"
                shift 2
                continue
                ;;
            '-l'|'--log-level')
                case "$2" in
                    DEBUG|INFO|WARN|ERROR)
                        RANDY_CONFIG["LOG_LEVEL"]="$2"
                        ;;
                    *)
                        echo -e "${RED}❌ ERROR: Invalid log level '$2'${NC}" >&2
                        echo -e "${YELLOW}Valid levels: DEBUG, INFO, WARN, ERROR${NC}" >&2
                        return 1
                        ;;
                esac
                shift 2
                continue
                ;;
            '-d'|'--dry-run')
                RANDY_CONFIG["DRY_RUN"]="true"
                shift
                continue
                ;;
            '-f'|'--force')
                RANDY_CONFIG["FORCE"]="true"
                shift
                continue
                ;;
            '-q'|'--quiet')
                RANDY_CONFIG["QUIET"]="true"
                shift
                continue
                ;;
            '-v'|'--verbose')
                RANDY_CONFIG["VERBOSE"]="true"
                shift
                continue
                ;;
            '-y'|'--yes')
                RANDY_CONFIG["AUTO_CONFIRM"]="true"
                shift
                continue
                ;;
            '-b'|'--backup')
                case "$2" in
                    true|false)
                        RANDY_CONFIG["BACKUP_ENABLED"]="$2"
                        ;;
                    *)
                        echo -e "${RED}❌ ERROR: Backup flag must be 'true' or 'false'${NC}" >&2
                        return 1
                        ;;
                esac
                shift 2
                continue
                ;;
            '--')
                shift
                break
                ;;
            *)
                echo -e "${RED}❌ ERROR: Internal error in argument parsing${NC}" >&2
                return 1
                ;;
        esac
    done
    
    # Randy's post-processing validation
    validate_randy_arguments
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Argument Validation Pattern
# ==================================================================================
validate_randy_arguments() {
    local errors=0
    
    # Required argument validation
    if [[ -z "${RANDY_CONFIG["SERVICE_NAME"]}" ]]; then
        echo -e "${RED}❌ ERROR: Service name is required (-s/--service)${NC}" >&2
        ((errors++))
    fi
    
    if [[ -z "${RANDY_CONFIG["OPERATION"]}" ]]; then
        echo -e "${RED}❌ ERROR: Operation is required (-o/--operation)${NC}" >&2
        echo -e "${YELLOW}Valid operations: install, uninstall, start, stop, restart, status, update${NC}" >&2
        ((errors++))
    else
        # Validate operation
        case "${RANDY_CONFIG["OPERATION"]}" in
            install|uninstall|start|stop|restart|status|update)
                # Valid operation
                ;;
            *)
                echo -e "${RED}❌ ERROR: Invalid operation '${RANDY_CONFIG["OPERATION"]}'${NC}" >&2
                echo -e "${YELLOW}Valid operations: install, uninstall, start, stop, restart, status, update${NC}" >&2
                ((errors++))
                ;;
        esac
    fi
    
    # Randy's conflict detection
    if [[ "${RANDY_CONFIG["QUIET"]}" == "true" && "${RANDY_CONFIG["VERBOSE"]}" == "true" ]]; then
        echo -e "${RED}❌ ERROR: Cannot use both --quiet and --verbose${NC}" >&2
        ((errors++))
    fi
    
    # Config file validation
    if [[ -n "${RANDY_CONFIG["CONFIG_FILE"]}" && ! -f "${RANDY_CONFIG["CONFIG_FILE"]}" ]]; then
        echo -e "${RED}❌ ERROR: Config file '${RANDY_CONFIG["CONFIG_FILE"]}' does not exist${NC}" >&2
        ((errors++))
    fi
    
    if [[ $errors -gt 0 ]]; then
        echo -e "${RED}❌ Found $errors validation errors${NC}" >&2
        show_randy_usage
        return 1
    fi
    
    return 0
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Usage Display Pattern
# ==================================================================================
show_randy_usage() {
    cat << 'EOF'
🎓 RANDY'S ADVANCED BASH EDUCATION - Module 1: Argument Parsing

SYNOPSIS:
    randy_bash_module1.sh [OPTIONS]

DESCRIPTION:
    Educational demonstration of Randy's advanced argument parsing techniques
    developed through 306+ commits of production service management evolution.

REQUIRED OPTIONS:
    -s, --service NAME       Service name to manage
    -o, --operation OP       Operation to perform
                            (install|uninstall|start|stop|restart|status|update)

OPTIONAL ARGUMENTS:
    -c, --config FILE        Configuration file path
    -l, --log-level LEVEL    Logging level (DEBUG|INFO|WARN|ERROR)
    -d, --dry-run           Show what would be done without executing
    -f, --force             Force operation without confirmations
    -q, --quiet             Suppress non-error output
    -v, --verbose           Enable detailed output
    -y, --yes               Auto-confirm all prompts
    -b, --backup BOOL       Enable/disable backups (true|false)

EXAMPLES:
    # Basic service management
    randy_bash_module1.sh -s nginx -o status

    # Advanced usage with options
    randy_bash_module1.sh --service myapp --operation install \\
                         --config /etc/myapp.conf --log-level DEBUG \\
                         --backup true --verbose

    # Dry run with auto-confirmation
    randy_bash_module1.sh -s redis -o restart -d -y

EDUCATIONAL NOTES:
    - Uses advanced getopt for robust parsing
    - Implements comprehensive validation
    - Handles argument conflicts intelligently
    - Provides detailed error feedback
    - Demonstrates production-grade patterns

EOF
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Configuration Display
# ==================================================================================
show_randy_config() {
    echo -e "${BLUE}📋 RANDY'S CONFIGURATION STATE:${NC}"
    echo -e "${CYAN}================================${NC}"
    
    for key in $(echo "${!RANDY_CONFIG[@]}" | tr ' ' '\n' | sort); do
        local value="${RANDY_CONFIG[$key]}"
        local display_value="$value"
        
        # Randy's value formatting
        if [[ -z "$value" ]]; then
            display_value="${YELLOW}(not set)${NC}"
        elif [[ "$value" == "true" ]]; then
            display_value="${GREEN}$value${NC}"
        elif [[ "$value" == "false" ]]; then
            display_value="${RED}$value${NC}"
        else
            display_value="${WHITE}$value${NC}"
        fi
        
        printf "${PURPLE}%-15s${NC}: %s\n" "$key" "$display_value"
    done
    echo ""
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Logging System
# ==================================================================================
randy_log() {
    local level="$1"
    local message="$2"
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    
    # Respect quiet mode
    [[ "${RANDY_CONFIG["QUIET"]}" == "true" && "$level" != "ERROR" ]] && return 0
    
    # Check log level filtering
    case "${RANDY_CONFIG["LOG_LEVEL"]}" in
        ERROR)
            [[ "$level" == "ERROR" ]] || return 0
            ;;
        WARN)
            [[ "$level" =~ ^(ERROR|WARN)$ ]] || return 0
            ;;
        INFO)
            [[ "$level" =~ ^(ERROR|WARN|INFO)$ ]] || return 0
            ;;
        DEBUG)
            # Allow all levels
            ;;
    esac
    
    case "$level" in
        DEBUG)
            echo -e "${PURPLE}[$timestamp] DEBUG: $message${NC}" >&2
            ;;
        INFO)
            echo -e "${CYAN}[$timestamp] INFO: $message${NC}"
            ;;
        WARN)
            echo -e "${YELLOW}[$timestamp] WARN: $message${NC}" >&2
            ;;
        ERROR)
            echo -e "${RED}[$timestamp] ERROR: $message${NC}" >&2
            ;;
    esac
}

# ==================================================================================
# EDUCATIONAL DEMO: Show Randy's Techniques in Action
# ==================================================================================
educational_demo() {
    echo -e "${GREEN}🎓 RANDY'S BASH MASTERY - Module 1 Educational Demo${NC}"
    echo -e "${CYAN}=================================================${NC}\n"
    
    randy_log "INFO" "Starting Randy's argument parsing demonstration"
    randy_log "DEBUG" "This debug message shows Randy's logging system"
    
    if parse_randy_arguments "$@"; then
        randy_log "INFO" "Arguments parsed successfully!"
        show_randy_config
        
        randy_log "INFO" "Operation: ${RANDY_CONFIG["OPERATION"]} on service: ${RANDY_CONFIG["SERVICE_NAME"]}"
        
        if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
            randy_log "WARN" "DRY RUN MODE: Would execute ${RANDY_CONFIG["OPERATION"]} operation"
        else
            randy_log "INFO" "Ready to execute ${RANDY_CONFIG["OPERATION"]} operation"
        fi
        
        echo -e "\n${GREEN}✅ EDUCATIONAL SUCCESS: Randy's argument parsing mastered!${NC}"
        echo -e "${YELLOW}Key Learning: This is production-grade argument handling built through crisis innovation${NC}\n"
    else
        randy_log "ERROR" "Failed to parse arguments"
        return 1
    fi
}

# ==================================================================================
# MAIN EXECUTION
# ==================================================================================
main() {
    if [[ $# -eq 0 ]]; then
        echo -e "${BLUE}🎓 Randy's Advanced Bash Education - Module 1${NC}"
        echo -e "${YELLOW}This module teaches argument parsing mastery from Randy's fire system${NC}\n"
        show_randy_usage
        echo -e "${GREEN}Try running with: $0 -s nginx -o status -v${NC}"
        return 0
    fi
    
    educational_demo "$@"
}

# Run if executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi