#!/usr/bin/env bash
# ==================================================================================
# RANDY'S ADVANCED BASH CONCEPTS - EDUCATIONAL MODULE 2: SYSTEMD MASTERY
# ==================================================================================
#
# CRISIS INNOVATION BACKGROUND:
# This module extracts Randy's systemd integration patterns developed through
# real-world service management challenges. Randy learned these techniques when
# building production-grade service automation under pressure.
#
# EDUCATIONAL VALUE:
# - Systemd service creation and management
# - Service status monitoring and health checking
# - Unit file generation and validation
# - Error handling and recovery patterns
# - Production-grade service lifecycle management
#
# EXTRACTED FROM: Randy's fire system - universal_service_manager.sh
# ==================================================================================

# Source Module 1 for argument parsing
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/randy_bash_module1_argument_parsing.sh"

# Randy's systemd configuration patterns
declare -A SYSTEMD_CONFIG=(
    ["UNIT_PATH"]="/etc/systemd/system"
    ["USER_UNIT_PATH"]="${HOME}/.config/systemd/user"
    ["SERVICE_TYPE"]="simple"
    ["RESTART_POLICY"]="always"
    ["RESTART_SEC"]="10"
    ["TIMEOUT_START"]="90"
    ["TIMEOUT_STOP"]="30"
    ["WANTED_BY"]="multi-user.target"
    ["REQUIRES"]=""
    ["AFTER"]=""
    ["BEFORE"]=""
    ["ENVIRONMENT"]=""
    ["EXEC_START_PRE"]=""
    ["EXEC_RELOAD"]=""
    ["EXEC_STOP"]=""
    ["USER"]=""
    ["GROUP"]=""
    ["WORKING_DIR"]=""
    ["PID_FILE"]=""
    ["UMASK"]="0022"
)

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Systemd Service Creation Pattern
# ==================================================================================
create_randy_systemd_service() {
    local service_name="$1"
    local exec_start="$2"
    local description="$3"
    local user_service="${4:-false}"
    
    randy_log "INFO" "Creating systemd service: $service_name"
    
    local unit_path
    if [[ "$user_service" == "true" ]]; then
        unit_path="${SYSTEMD_CONFIG["USER_UNIT_PATH"]}"
        mkdir -p "$unit_path"
    else
        unit_path="${SYSTEMD_CONFIG["UNIT_PATH"]}"
    fi
    
    local service_file="${unit_path}/${service_name}.service"
    
    if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
        randy_log "INFO" "DRY RUN: Would create service file at $service_file"
        show_randy_service_template "$service_name" "$exec_start" "$description"
        return 0
    fi
    
    # Randy's backup strategy
    if [[ -f "$service_file" && "${RANDY_CONFIG["BACKUP_ENABLED"]}" == "true" ]]; then
        local backup_file="${service_file}.backup.$(date +%Y%m%d_%H%M%S)"
        randy_log "INFO" "Backing up existing service file to: $backup_file"
        cp "$service_file" "$backup_file"
    fi
    
    # Generate Randy's service template
    generate_randy_service_file "$service_file" "$service_name" "$exec_start" "$description"
    
    # Validate the service file
    if validate_randy_service_file "$service_file"; then
        randy_log "INFO" "Service file created successfully: $service_file"
        
        # Reload systemd daemon
        if [[ "$user_service" == "true" ]]; then
            systemctl --user daemon-reload
        else
            systemctl daemon-reload
        fi
        
        randy_log "INFO" "Systemd daemon reloaded"
        return 0
    else
        randy_log "ERROR" "Service file validation failed"
        return 1
    fi
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Service File Generation
# ==================================================================================
generate_randy_service_file() {
    local service_file="$1"
    local service_name="$2"
    local exec_start="$3"
    local description="$4"
    
    randy_log "DEBUG" "Generating service file: $service_file"
    
    cat > "$service_file" << EOF
# ======================================================================
# Randy's Auto-Generated Systemd Service File
# Service: $service_name
# Generated: $(date)
# ======================================================================

[Unit]
Description=$description
Documentation=https://github.com/randy/educational-tools
After=network.target${SYSTEMD_CONFIG["AFTER"]:+ ${SYSTEMD_CONFIG["AFTER"]}}
${SYSTEMD_CONFIG["REQUIRES"]:+Requires=${SYSTEMD_CONFIG["REQUIRES"]}}
${SYSTEMD_CONFIG["BEFORE"]:+Before=${SYSTEMD_CONFIG["BEFORE"]}}

[Service]
Type=${SYSTEMD_CONFIG["SERVICE_TYPE"]}
ExecStart=$exec_start
${SYSTEMD_CONFIG["EXEC_START_PRE"]:+ExecStartPre=${SYSTEMD_CONFIG["EXEC_START_PRE"]}}
${SYSTEMD_CONFIG["EXEC_RELOAD"]:+ExecReload=${SYSTEMD_CONFIG["EXEC_RELOAD"]}}
${SYSTEMD_CONFIG["EXEC_STOP"]:+ExecStop=${SYSTEMD_CONFIG["EXEC_STOP"]}}
Restart=${SYSTEMD_CONFIG["RESTART_POLICY"]}
RestartSec=${SYSTEMD_CONFIG["RESTART_SEC"]}
TimeoutStartSec=${SYSTEMD_CONFIG["TIMEOUT_START"]}
TimeoutStopSec=${SYSTEMD_CONFIG["TIMEOUT_STOP"]}
${SYSTEMD_CONFIG["USER"]:+User=${SYSTEMD_CONFIG["USER"]}}
${SYSTEMD_CONFIG["GROUP"]:+Group=${SYSTEMD_CONFIG["GROUP"]}}
${SYSTEMD_CONFIG["WORKING_DIR"]:+WorkingDirectory=${SYSTEMD_CONFIG["WORKING_DIR"]}}
${SYSTEMD_CONFIG["PID_FILE"]:+PIDFile=${SYSTEMD_CONFIG["PID_FILE"]}}
${SYSTEMD_CONFIG["ENVIRONMENT"]:+Environment=${SYSTEMD_CONFIG["ENVIRONMENT"]}}
UMask=${SYSTEMD_CONFIG["UMASK"]}

# Randy's security hardening
NoNewPrivileges=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/var/log /tmp
PrivateTmp=true
PrivateDevices=true
ProtectKernelTunables=true
ProtectControlGroups=true
RestrictRealtime=true

[Install]
WantedBy=${SYSTEMD_CONFIG["WANTED_BY"]}
EOF
    
    randy_log "DEBUG" "Service file generated successfully"
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Service Validation Pattern
# ==================================================================================
validate_randy_service_file() {
    local service_file="$1"
    
    randy_log "DEBUG" "Validating service file: $service_file"
    
    # Check if file exists and is readable
    if [[ ! -f "$service_file" ]]; then
        randy_log "ERROR" "Service file does not exist: $service_file"
        return 1
    fi
    
    if [[ ! -r "$service_file" ]]; then
        randy_log "ERROR" "Service file is not readable: $service_file"
        return 1
    fi
    
    # Validate using systemd-analyze (Randy's production check)
    if command -v systemd-analyze >/dev/null 2>&1; then
        local service_name="$(basename "$service_file" .service)"
        if systemd-analyze verify "$service_file" 2>/dev/null; then
            randy_log "INFO" "Service file passed systemd validation"
            return 0
        else
            randy_log "WARN" "Service file has systemd warnings (still usable)"
            if [[ "${RANDY_CONFIG["VERBOSE"]}" == "true" ]]; then
                randy_log "DEBUG" "Systemd validation output:"
                systemd-analyze verify "$service_file" 2>&1 | while read -r line; do
                    randy_log "DEBUG" "  $line"
                done
            fi
            return 0  # Still proceed despite warnings
        fi
    else
        randy_log "WARN" "systemd-analyze not available, skipping validation"
        return 0
    fi
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Service Management Pattern
# ==================================================================================
manage_randy_service() {
    local operation="$1"
    local service_name="$2"
    local user_service="${3:-false}"
    
    local systemctl_cmd="systemctl"
    if [[ "$user_service" == "true" ]]; then
        systemctl_cmd="systemctl --user"
    fi
    
    randy_log "INFO" "Managing service '$service_name' with operation: $operation"
    
    case "$operation" in
        "start")
            if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
                randy_log "INFO" "DRY RUN: Would start service $service_name"
            else
                randy_log "INFO" "Starting service: $service_name"
                $systemctl_cmd start "$service_name"
                sleep 2  # Randy's stabilization wait
                check_randy_service_status "$service_name" "$user_service"
            fi
            ;;
        "stop")
            if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
                randy_log "INFO" "DRY RUN: Would stop service $service_name"
            else
                randy_log "INFO" "Stopping service: $service_name"
                $systemctl_cmd stop "$service_name"
            fi
            ;;
        "restart")
            if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
                randy_log "INFO" "DRY RUN: Would restart service $service_name"
            else
                randy_log "INFO" "Restarting service: $service_name"
                $systemctl_cmd restart "$service_name"
                sleep 3  # Randy's post-restart wait
                check_randy_service_status "$service_name" "$user_service"
            fi
            ;;
        "enable")
            if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
                randy_log "INFO" "DRY RUN: Would enable service $service_name"
            else
                randy_log "INFO" "Enabling service: $service_name"
                $systemctl_cmd enable "$service_name"
            fi
            ;;
        "disable")
            if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
                randy_log "INFO" "DRY RUN: Would disable service $service_name"
            else
                randy_log "INFO" "Disabling service: $service_name"
                $systemctl_cmd disable "$service_name"
            fi
            ;;
        "status")
            check_randy_service_status "$service_name" "$user_service"
            ;;
        *)
            randy_log "ERROR" "Unknown operation: $operation"
            return 1
            ;;
    esac
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Randy's Service Status Checking
# ==================================================================================
check_randy_service_status() {
    local service_name="$1"
    local user_service="${2:-false}"
    
    local systemctl_cmd="systemctl"
    if [[ "$user_service" == "true" ]]; then
        systemctl_cmd="systemctl --user"
    fi
    
    randy_log "INFO" "Checking status of service: $service_name"
    
    # Randy's comprehensive status check
    local active_state
    active_state=$($systemctl_cmd is-active "$service_name" 2>/dev/null || echo "unknown")
    
    local enabled_state
    enabled_state=$($systemctl_cmd is-enabled "$service_name" 2>/dev/null || echo "unknown")
    
    echo -e "\n${BLUE}📊 RANDY'S SERVICE STATUS REPORT:${NC}"
    echo -e "${CYAN}================================${NC}"
    echo -e "${PURPLE}Service:${NC} ${WHITE}$service_name${NC}"
    
    case "$active_state" in
        "active")
            echo -e "${PURPLE}Status:${NC} ${GREEN}$active_state ✅${NC}"
            ;;
        "inactive")
            echo -e "${PURPLE}Status:${NC} ${YELLOW}$active_state ⏸️${NC}"
            ;;
        "failed")
            echo -e "${PURPLE}Status:${NC} ${RED}$active_state ❌${NC}"
            ;;
        *)
            echo -e "${PURPLE}Status:${NC} ${YELLOW}$active_state ❓${NC}"
            ;;
    esac
    
    case "$enabled_state" in
        "enabled")
            echo -e "${PURPLE}Enabled:${NC} ${GREEN}$enabled_state ✅${NC}"
            ;;
        "disabled")
            echo -e "${PURPLE}Enabled:${NC} ${RED}$enabled_state ❌${NC}"
            ;;
        *)
            echo -e "${PURPLE}Enabled:${NC} ${YELLOW}$enabled_state ❓${NC}"
            ;;
    esac
    
    # Randy's detailed status if verbose
    if [[ "${RANDY_CONFIG["VERBOSE"]}" == "true" ]]; then
        echo -e "\n${BLUE}📝 DETAILED STATUS:${NC}"
        $systemctl_cmd status "$service_name" --no-pager -l || true
    fi
    
    # Randy's log tail if service is problematic
    if [[ "$active_state" == "failed" && "${RANDY_CONFIG["VERBOSE"]}" == "true" ]]; then
        echo -e "\n${RED}💥 RECENT ERROR LOGS:${NC}"
        journalctl $(if [[ "$user_service" == "true" ]]; then echo "--user"; fi) \
                   -u "$service_name" -n 10 --no-pager || true
    fi
    
    echo ""
}

# ==================================================================================
# EDUCATIONAL FUNCTION: Show Service Template
# ==================================================================================
show_randy_service_template() {
    local service_name="$1"
    local exec_start="$2"
    local description="$3"
    
    echo -e "\n${BLUE}📄 RANDY'S SERVICE TEMPLATE PREVIEW:${NC}"
    echo -e "${CYAN}====================================${NC}"
    
    cat << EOF
[Unit]
Description=$description
After=network.target
Documentation=https://github.com/randy/educational-tools

[Service]
Type=${SYSTEMD_CONFIG["SERVICE_TYPE"]}
ExecStart=$exec_start
Restart=${SYSTEMD_CONFIG["RESTART_POLICY"]}
RestartSec=${SYSTEMD_CONFIG["RESTART_SEC"]}
TimeoutStartSec=${SYSTEMD_CONFIG["TIMEOUT_START"]}
TimeoutStopSec=${SYSTEMD_CONFIG["TIMEOUT_STOP"]}
UMask=${SYSTEMD_CONFIG["UMASK"]}

# Randy's security hardening
NoNewPrivileges=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=/var/log /tmp
PrivateTmp=true

[Install]
WantedBy=${SYSTEMD_CONFIG["WANTED_BY"]}
EOF
    echo ""
}

# ==================================================================================
# EDUCATIONAL DEMO: Randy's Systemd Integration
# ==================================================================================
randy_systemd_demo() {
    echo -e "${GREEN}🎓 RANDY'S SYSTEMD MASTERY - Module 2 Educational Demo${NC}"
    echo -e "${CYAN}===================================================${NC}\n"
    
    local service_name="${RANDY_CONFIG["SERVICE_NAME"]}"
    local operation="${RANDY_CONFIG["OPERATION"]}"
    
    case "$operation" in
        "install")
            randy_log "INFO" "Installing systemd service for: $service_name"
            
            # Example service installation
            local exec_start="/usr/local/bin/$service_name"
            local description="Randy's Educational Service: $service_name"
            
            if create_randy_systemd_service "$service_name" "$exec_start" "$description"; then
                randy_log "INFO" "Service installation completed"
                manage_randy_service "enable" "$service_name"
                echo -e "\n${GREEN}✅ EDUCATIONAL SUCCESS: Service installed and enabled!${NC}"
            else
                randy_log "ERROR" "Service installation failed"
                return 1
            fi
            ;;
        "uninstall")
            randy_log "INFO" "Uninstalling systemd service: $service_name"
            
            manage_randy_service "stop" "$service_name"
            manage_randy_service "disable" "$service_name"
            
            local service_file="${SYSTEMD_CONFIG["UNIT_PATH"]}/${service_name}.service"
            if [[ -f "$service_file" ]]; then
                if [[ "${RANDY_CONFIG["DRY_RUN"]}" == "true" ]]; then
                    randy_log "INFO" "DRY RUN: Would remove $service_file"
                else
                    randy_log "INFO" "Removing service file: $service_file"
                    rm -f "$service_file"
                    systemctl daemon-reload
                fi
            fi
            
            echo -e "\n${GREEN}✅ EDUCATIONAL SUCCESS: Service uninstalled!${NC}"
            ;;
        "start"|"stop"|"restart"|"status")
            manage_randy_service "$operation" "$service_name"
            ;;
        *)
            randy_log "ERROR" "Unknown operation for systemd demo: $operation"
            return 1
            ;;
    esac
    
    echo -e "${YELLOW}Key Learning: This is production-grade systemd management from Randy's fire system${NC}\n"
}

# ==================================================================================
# MAIN EXECUTION
# ==================================================================================
main() {
    if [[ $# -eq 0 ]]; then
        echo -e "${BLUE}🎓 Randy's Advanced Bash Education - Module 2: Systemd Mastery${NC}"
        echo -e "${YELLOW}This module teaches systemd integration from Randy's fire system${NC}\n"
        show_randy_usage
        echo -e "${GREEN}Try: $0 -s test-service -o install -d -v${NC}"
        echo -e "${GREEN}Or: $0 -s nginx -o status${NC}"
        return 0
    fi
    
    if parse_randy_arguments "$@"; then
        randy_systemd_demo
    else
        return 1
    fi
}

# Run if executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi