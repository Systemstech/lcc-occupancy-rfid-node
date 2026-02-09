# Git Setup and Push Instructions

Follow these steps to push your project to GitHub:

## Step 1: Create GitHub Repository

1. Go to https://github.com and log in
2. Click the "+" in the top right, select "New repository"
3. Name it: `lcc-occupancy-rfid-node` (or your preferred name)
4. Leave it **empty** (don't add README, .gitignore, or license)
5. Click "Create repository"
6. Copy the repository URL (will be like: `https://github.com/YOUR_USERNAME/lcc-occupancy-rfid-node.git`)

## Step 2: Organize Your Files

Make sure your project folder has this structure:

```
lcc_occupancy_rfid_node/
├── lcc_occupancy_rfid_node.ino
├── platformio.ini
├── README.md
├── WIRING_DIAGRAM.md
├── WIRING.md
├── DIRECTION_DETECTION.md
├── AXLE_COUNTING.md
├── .gitignore
├── LICENSE
└── README_GIT.md (rename this to replace README.md if you want the Git version)
```

## Step 3: Initialize Git and Push

Open a terminal/command prompt in your project folder and run these commands:

### For Windows (Command Prompt or PowerShell):
```bash
# Navigate to your project folder
cd C:\Users\lookw\OneDrive\Documents\Arduino\LCC_Node\lcc_occupancy_rfid_node

# Initialize git repository
git init

# Add all files
git add .

# Create first commit
git commit -m "Initial commit - ESP32 LCC Occupancy + RFID Node"

# Add your GitHub repository as remote (replace with YOUR URL)
git remote add origin https://github.com/YOUR_USERNAME/lcc-occupancy-rfid-node.git

# Push to GitHub
git branch -M main
git push -u origin main
```

### For Mac/Linux:
```bash
# Navigate to your project folder
cd ~/Documents/Arduino/LCC_Node/lcc_occupancy_rfid_node

# Initialize git repository
git init

# Add all files
git add .

# Create first commit
git commit -m "Initial commit - ESP32 LCC Occupancy + RFID Node"

# Add your GitHub repository as remote (replace with YOUR URL)
git remote add origin https://github.com/YOUR_USERNAME/lcc-occupancy-rfid-node.git

# Push to GitHub
git branch -M main
git push -u origin main
```

## Step 4: Verify

Go to your GitHub repository URL in a browser and verify all files are there!

## Future Updates

After making changes to your code, push updates with:

```bash
# Stage your changes
git add .

# Commit with a message describing what you changed
git commit -m "Description of your changes"

# Push to GitHub
git push
```

## Common Git Commands

```bash
# Check status of your files
git status

# See what changed
git diff

# View commit history
git log

# Create a new branch for experiments
git checkout -b experimental-feature

# Switch back to main branch
git checkout main

# Pull latest changes from GitHub
git pull
```

## Troubleshooting

### "Authentication failed" error:
- GitHub removed password authentication
- You need to use a **Personal Access Token** instead
- Go to GitHub → Settings → Developer settings → Personal access tokens
- Generate a token with "repo" permissions
- Use the token as your password when pushing

### "Updates were rejected" error:
```bash
# Pull first, then push
git pull origin main --rebase
git push
```

### Want to undo last commit (before pushing):
```bash
git reset --soft HEAD~1
```

## Optional: Add README Badge

After pushing, you can add a badge to your README:

```markdown
![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-ESP32-blue)
```

## Need Help?

- Git basics: https://git-scm.com/book/en/v2
- GitHub guides: https://guides.github.com/
- Git cheat sheet: https://education.github.com/git-cheat-sheet-education.pdf
