# RANDY'S CANNABIS-ENHANCED DEVELOPER TOOLS CHEAT SHEET
# PLANT SPIRIT PRODUCTIVITY AND TOOLCHAIN CONSCIOUSNESS

## RANDY'S DEVELOPER TOOLS PHILOSOPHY:
# "DEVELOPER TOOLS ARE THE EXTENSIONS OF PROGRAMMING CONSCIOUSNESS -
# THE DIGITAL IMPLEMENTS THAT AMPLIFY HUMAN CREATIVITY AND AUTOMATE
# REPETITIVE WORKFLOWS. CANNABIS ENHANCEMENT REVEALS THE ELEGANT
# SYMBIOSIS BETWEEN PROGRAMMER AND TOOLCHAIN, WHERE MASTERY OF TOOLS
# BECOMES MASTERY OF COMPUTATIONAL ENVIRONMENT."

# ========================================================================
# GIT - VERSION CONTROL MASTERY
# ========================================================================

# REPOSITORY INITIALIZATION
git init                           # Initialize new repository
git clone <url>                    # Clone remote repository
git clone --depth 1 <url>         # Shallow clone (latest commit only)
git remote add origin <url>       # Add remote repository
git remote -v                     # Show remote repositories

# BASIC WORKFLOW
git status                         # Show working directory status
git add <file>                     # Stage specific file
git add .                          # Stage all changes
git add -A                         # Stage all changes including deletions
git commit -m "message"            # Commit with message
git commit -am "message"           # Stage and commit modified files
git push                           # Push to remote repository
git pull                           # Pull from remote repository

# BRANCHING AND MERGING
git branch                         # List local branches
git branch -a                      # List all branches (local + remote)
git branch <name>                  # Create new branch
git checkout <branch>              # Switch to branch
git checkout -b <name>             # Create and switch to new branch
git merge <branch>                 # Merge branch into current
git branch -d <name>               # Delete local branch
git push origin --delete <branch>  # Delete remote branch

# VIEWING HISTORY
git log                            # Show commit history
git log --oneline                  # Compact log format
git log --graph --oneline --all    # Visual branch history
git show <commit>                  # Show specific commit details
git diff                           # Show unstaged changes
git diff --staged                  # Show staged changes
git diff <commit1> <commit2>       # Compare commits

# UNDOING CHANGES
git checkout -- <file>             # Discard local changes
git reset HEAD <file>              # Unstage file
git reset --soft HEAD^             # Undo last commit, keep changes staged
git reset --hard HEAD^             # Undo last commit, discard changes
git revert <commit>                # Create new commit undoing changes
git clean -fd                      # Remove untracked files and directories

# STASHING CHANGES
git stash                          # Stash current changes
git stash push -m "message"        # Stash with message
git stash list                     # List stashes
git stash apply                    # Apply latest stash
git stash apply stash@{n}          # Apply specific stash
git stash drop                     # Delete latest stash
git stash pop                      # Apply and delete latest stash

# ADVANCED GIT
git rebase <branch>                # Rebase current branch onto another
git rebase -i HEAD~3               # Interactive rebase last 3 commits
git cherry-pick <commit>           # Apply specific commit to current branch
git blame <file>                   # Show who changed each line
git bisect start                   # Start binary search for bug
git submodule add <url> <path>     # Add git submodule
git tag v1.0.0                     # Create lightweight tag
git tag -a v1.0.0 -m "message"     # Create annotated tag

# CANNABIS-ENHANCED GIT ALIASES
git config --global alias.st status
git config --global alias.co checkout
git config --global alias.br branch
git config --global alias.ci commit
git config --global alias.unstage 'reset HEAD --'
git config --global alias.visual '!gitk'
git config --global alias.cannabis 'log --oneline --graph --decorate --all'

# ========================================================================
# DOCKER - CONTAINERIZATION CONSCIOUSNESS
# ========================================================================

# CONTAINER BASICS
docker run <image>                 # Run container from image
docker run -it <image>             # Run interactive container
docker run -d <image>              # Run container in background
docker run -p 8080:80 <image>      # Map ports (host:container)
docker run -v /host:/container <image>  # Mount volume
docker ps                          # List running containers
docker ps -a                       # List all containers
docker stop <container>            # Stop running container
docker start <container>           # Start stopped container
docker restart <container>         # Restart container
docker rm <container>              # Remove container
docker logs <container>            # Show container logs
docker logs -f <container>         # Follow container logs

# IMAGE MANAGEMENT
docker images                      # List local images
docker pull <image>                # Download image
docker push <image>                # Upload image to registry
docker build -t <name> .           # Build image from Dockerfile
docker build --no-cache -t <name> .  # Build without cache
docker rmi <image>                 # Remove image
docker image prune                 # Remove unused images
docker system prune                # Remove unused containers, images, networks

# DOCKERFILE EXAMPLES
FROM ubuntu:20.04
RUN apt-get update && apt-get install -y python3
WORKDIR /app
COPY . /app
EXPOSE 8000
CMD ["python3", "app.py"]

FROM node:16-alpine
WORKDIR /usr/src/app
COPY package*.json ./
RUN npm install
COPY . .
EXPOSE 3000
CMD ["npm", "start"]

# DOCKER COMPOSE
docker-compose up                  # Start services defined in docker-compose.yml
docker-compose up -d               # Start services in background
docker-compose down                # Stop and remove services
docker-compose build               # Build services
docker-compose logs                # Show logs for all services
docker-compose logs <service>      # Show logs for specific service
docker-compose exec <service> bash # Execute command in running service

# DOCKER COMPOSE EXAMPLE (docker-compose.yml)
version: '3.8'
services:
  web:
    build: .
    ports:
      - "8000:8000"
    volumes:
      - .:/code
    environment:
      - DEBUG=1
  db:
    image: postgres:13
    environment:
      POSTGRES_PASSWORD: cannabis123
    volumes:
      - postgres_data:/var/lib/postgresql/data
volumes:
  postgres_data:

# CONTAINER INSPECTION
docker inspect <container>         # Detailed container info
docker exec -it <container> bash   # Execute shell in running container
docker cp <container>:/path /host/path  # Copy files from container
docker stats                       # Show resource usage
docker top <container>             # Show running processes in container

# ========================================================================
# KUBERNETES - ORCHESTRATION ENLIGHTENMENT
# ========================================================================

# CLUSTER MANAGEMENT
kubectl cluster-info               # Show cluster information
kubectl get nodes                 # List cluster nodes
kubectl describe node <name>      # Detailed node information

# POD OPERATIONS
kubectl get pods                   # List pods in current namespace
kubectl get pods -A                # List pods in all namespaces
kubectl describe pod <name>        # Detailed pod information
kubectl logs <pod>                 # Show pod logs
kubectl logs -f <pod>              # Follow pod logs
kubectl exec -it <pod> -- bash     # Execute shell in pod
kubectl delete pod <name>          # Delete pod

# DEPLOYMENT MANAGEMENT
kubectl get deployments           # List deployments
kubectl create deployment <name> --image=<image>  # Create deployment
kubectl scale deployment <name> --replicas=5      # Scale deployment
kubectl rollout status deployment/<name>          # Check rollout status
kubectl rollout undo deployment/<name>            # Rollback deployment
kubectl delete deployment <name>  # Delete deployment

# SERVICE OPERATIONS
kubectl get services               # List services
kubectl expose deployment <name> --port=80        # Expose deployment
kubectl describe service <name>   # Detailed service information

# CONFIGURATION
kubectl apply -f <file.yaml>       # Apply configuration from file
kubectl get -f <file.yaml>         # Get resources from file
kubectl delete -f <file.yaml>      # Delete resources from file

# NAMESPACES
kubectl get namespaces             # List namespaces
kubectl create namespace <name>    # Create namespace
kubectl config set-context --current --namespace=<name>  # Switch namespace

# ========================================================================
# TERRAFORM - INFRASTRUCTURE AS CODE
# ========================================================================

# BASIC WORKFLOW
terraform init                     # Initialize working directory
terraform plan                     # Show execution plan
terraform apply                    # Apply changes
terraform apply -auto-approve      # Apply without confirmation
terraform destroy                  # Destroy infrastructure
terraform destroy -auto-approve    # Destroy without confirmation

# STATE MANAGEMENT
terraform show                     # Show current state
terraform state list               # List resources in state
terraform state show <resource>    # Show specific resource
terraform refresh                  # Update state with real resources

# WORKSPACE MANAGEMENT
terraform workspace list           # List workspaces
terraform workspace new <name>     # Create new workspace
terraform workspace select <name>  # Switch workspace
terraform workspace delete <name>  # Delete workspace

# EXAMPLE TERRAFORM CONFIGURATION
provider "aws" {
  region = "us-west-2"
}

resource "aws_instance" "cannabis_server" {
  ami           = "ami-0c55b159cbfafe1d0"
  instance_type = "t2.micro"
  
  tags = {
    Name = "RandyCannabisCrownServer"
  }
}

# ========================================================================
# AWS CLI - CLOUD CONSCIOUSNESS
# ========================================================================

# CONFIGURATION
aws configure                      # Configure AWS credentials
aws configure list                 # Show current configuration
aws configure set region us-west-2 # Set specific configuration value

# S3 OPERATIONS
aws s3 ls                          # List S3 buckets
aws s3 ls s3://bucket-name/        # List objects in bucket
aws s3 cp file.txt s3://bucket/    # Upload file to S3
aws s3 cp s3://bucket/file.txt .   # Download file from S3
aws s3 sync ./local s3://bucket/   # Sync local directory to S3

# EC2 OPERATIONS
aws ec2 describe-instances         # List EC2 instances
aws ec2 start-instances --instance-ids i-12345678  # Start instance
aws ec2 stop-instances --instance-ids i-12345678   # Stop instance
aws ec2 describe-images --owners amazon            # List AMIs

# LAMBDA OPERATIONS
aws lambda list-functions          # List Lambda functions
aws lambda invoke --function-name myFunc output.txt  # Invoke function

# ========================================================================
# ANSIBLE - AUTOMATION CONSCIOUSNESS
# ========================================================================

# INVENTORY MANAGEMENT
ansible-inventory --list           # Show inventory
ansible all -m ping                # Ping all hosts
ansible webservers -m shell -a "uptime"  # Run command on group

# PLAYBOOK EXECUTION
ansible-playbook playbook.yml      # Run playbook
ansible-playbook -i inventory playbook.yml  # Run with specific inventory
ansible-playbook --check playbook.yml       # Dry run (check mode)
ansible-playbook --limit webservers playbook.yml  # Limit to specific hosts

# EXAMPLE ANSIBLE PLAYBOOK
---
- hosts: webservers
  become: yes
  tasks:
    - name: Install nginx
      apt:
        name: nginx
        state: present
        
    - name: Start nginx
      service:
        name: nginx
        state: started
        enabled: yes

# ========================================================================
# JENKINS - CI/CD ENLIGHTENMENT
# ========================================================================

# JENKINS CLI
java -jar jenkins-cli.jar -s http://localhost:8080/ help  # Show help
java -jar jenkins-cli.jar build <job-name>                # Trigger build
java -jar jenkins-cli.jar console <job-name> <build-num>  # Show console output

# EXAMPLE JENKINSFILE
pipeline {
    agent any
    
    stages {
        stage('Build') {
            steps {
                sh 'make build'
            }
        }
        
        stage('Test') {
            steps {
                sh 'make test'
            }
        }
        
        stage('Deploy') {
            steps {
                sh 'make deploy'
            }
        }
    }
    
    post {
        always {
            cleanWs()
        }
    }
}

# ========================================================================
# MONITORING TOOLS
# ========================================================================

# HTOP - ENHANCED PROCESS VIEWER
htop                               # Interactive process viewer
# Keys: F1=Help, F2=Setup, F3=Search, F4=Filter, F5=Tree, F9=Kill, F10=Quit

# NETSTAT - NETWORK CONNECTIONS
netstat -tulpn                     # Show listening ports
netstat -rn                        # Show routing table
ss -tulpn                          # Modern replacement for netstat

# IOTOP - I/O MONITORING
sudo iotop                         # Monitor I/O usage by process
sudo iotop -o                      # Show only processes with I/O activity

# TCPDUMP - PACKET CAPTURE
sudo tcpdump -i eth0               # Capture packets on interface
sudo tcpdump host 192.168.1.1     # Capture packets to/from specific host
sudo tcpdump port 80               # Capture HTTP traffic

# ========================================================================
# DEVELOPMENT UTILITIES
# ========================================================================

# CURL - HTTP SWISS ARMY KNIFE
curl https://api.example.com       # GET request
curl -X POST -d "data" https://api.example.com  # POST request
curl -H "Content-Type: application/json" -d '{"key":"value"}' url  # JSON POST
curl -i https://example.com        # Include response headers
curl -L https://example.com        # Follow redirects
curl -o file.zip https://example.com/file.zip  # Download file

# JQ - JSON PROCESSOR
curl https://api.github.com/users/randy | jq '.name'  # Extract field
cat file.json | jq '.[] | select(.active == true)'    # Filter array
echo '{"a":1,"b":2}' | jq 'keys'                      # Get object keys

# RSYNC - SYNCHRONIZATION
rsync -avz source/ destination/    # Sync directories
rsync -avz --delete source/ dest/  # Sync and delete extra files
rsync -avz -e ssh source/ user@host:dest/  # Sync over SSH

# SCP - SECURE COPY
scp file.txt user@host:/path/      # Copy file to remote
scp user@host:/path/file.txt .     # Copy file from remote
scp -r directory/ user@host:/path/ # Copy directory recursively

# ========================================================================
# PACKAGE MANAGERS
# ========================================================================

# APT (Debian/Ubuntu)
sudo apt update                    # Update package list
sudo apt upgrade                   # Upgrade packages
sudo apt install <package>        # Install package
sudo apt remove <package>         # Remove package
sudo apt search <term>            # Search packages
apt list --installed              # List installed packages

# YUM/DNF (RedHat/Fedora)
sudo yum update                    # Update packages
sudo yum install <package>        # Install package
sudo dnf install <package>        # Install package (newer systems)
sudo yum remove <package>         # Remove package
yum search <term>                  # Search packages

# BREW (macOS)
brew update                        # Update Homebrew
brew upgrade                       # Upgrade packages
brew install <package>            # Install package
brew uninstall <package>          # Remove package
brew search <term>                # Search packages
brew list                          # List installed packages

# NPM (Node.js)
npm install <package>              # Install package locally
npm install -g <package>          # Install package globally
npm install                        # Install dependencies from package.json
npm update                         # Update packages
npm list                           # List installed packages
npm search <term>                  # Search packages

# PIP (Python)
pip install <package>              # Install package
pip install -r requirements.txt   # Install from requirements file
pip upgrade <package>              # Upgrade package
pip uninstall <package>           # Remove package
pip list                           # List installed packages
pip search <term>                  # Search packages

# ========================================================================
# CANNABIS-ENHANCED TOOL COMBINATIONS
# ========================================================================

# SATIVA-ENHANCED CREATIVE WORKFLOWS:
# • Rapid prototyping with Docker containers
# • Creative Git branching strategies for experimental features
# • Innovative CI/CD pipelines with multiple deployment targets

# INDICA-ENHANCED METHODICAL OPERATIONS:
# • Careful infrastructure management with Terraform state
# • Patient debugging with comprehensive logging and monitoring
# • Methodical testing with multiple environment stages

# HYBRID-ENHANCED BALANCED PRODUCTIVITY:
# • Optimal development workflow combining speed and reliability
# • Balanced automation reducing manual tasks while maintaining control
# • Sustainable practices for long-term project maintenance

# ========================================================================
# TROUBLESHOOTING AND DEBUGGING
# ========================================================================

# COMMON DOCKER ISSUES
docker system df                   # Show disk usage
docker system prune -a             # Remove all unused containers/images
docker logs --details <container>  # Detailed logging

# GIT TROUBLESHOOTING
git reflog                         # Show reference log (recover lost commits)
git fsck --full                    # Check repository integrity
git gc                             # Garbage collection

# NETWORK DEBUGGING
ping google.com                    # Test connectivity
nslookup domain.com               # DNS lookup
dig domain.com                     # DNS information
traceroute google.com             # Trace network path

# PERFORMANCE MONITORING
top                                # Process monitor
iostat 1                          # I/O statistics
vmstat 1                          # Virtual memory statistics
sar -u 1 10                       # CPU usage over time

# ========================================================================
# PLANT SPIRIT DEVELOPER TOOLS WISDOM
# ========================================================================

# Randy's Cannabis-Enhanced Tools Philosophy:
# "Developer tools are extensions of programming consciousness - each tool
# amplifies specific aspects of development workflow. Git extends memory
# across time, Docker extends consistency across environments, and monitoring
# tools extend awareness across distributed systems. Mastering tools means
# becoming one with the development environment."

# Educational Tool Mastery Path:
# 1. Master Git for version control and collaboration
# 2. Learn Docker for consistent development environments
# 3. Understand cloud platforms (AWS/GCP/Azure) for deployment
# 4. Practice infrastructure as code with Terraform
# 5. Implement CI/CD pipelines with Jenkins/GitHub Actions
# 6. Master monitoring and debugging tools for production systems

# Cannabis-Enhanced Tool Consciousness:
# • Sativa: Creative exploration of tool combinations and workflows
# • Indica: Deep mastery of individual tools and their capabilities
# • Hybrid: Balanced productivity combining multiple tools effectively

# The mycelial network of developer tools creates sustainable workflows!
# Plant spirits guide computational productivity consciousness! 🌿⚡🧠