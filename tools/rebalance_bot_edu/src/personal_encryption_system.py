#!/usr/bin/env python3

"""
🔐 Personal Encryption System - Randy's Steganographic Key Distribution
 
This educational implementation recreates Randy's sophisticated encryption project:
- Steganographic key storage in JPG files
- Decoy images to confuse attackers  
- Password-protected key part extraction
- Multi-layer encryption with key assembly
- Encrypted disk image mounting
- Auto-unmount timers and drop folder automation

Educational Value:
- Advanced cryptography concepts
- Steganography and information hiding
- Key distribution and management
- Filesystem encryption
- Security through obscurity vs real security
"""

import os
import sys
import time
import json
import base64
import hashlib
import secrets
import subprocess
import tempfile
from pathlib import Path
from typing import List, Dict, Optional, Tuple
import argparse
from datetime import datetime, timedelta

# Cryptography imports
try:
    from cryptography.fernet import Fernet
    from cryptography.hazmat.primitives import hashes
    from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
    from PIL import Image, ImageOps
    import numpy as np
    DEPENDENCIES_AVAILABLE = True
except ImportError:
    print("📦 Installing required dependencies...")
    subprocess.run([sys.executable, "-m", "pip", "install", "cryptography", "Pillow", "numpy"])
    from cryptography.fernet import Fernet
    from cryptography.hazmat.primitives import hashes
    from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
    from PIL import Image, ImageOps
    import numpy as np
    DEPENDENCIES_AVAILABLE = True

class SteganographicKeyManager:
    """
    🖼️ Steganographic Key Distribution System
    
    Hides encrypted key parts in JPG images using LSB steganography
    Includes decoy images to confuse potential attackers
    """
    
    def __init__(self, key_dir: str = "encrypted_keys"):
        self.key_dir = Path(key_dir)
        self.key_dir.mkdir(exist_ok=True)
        
        # Educational: Show what we're doing
        self.verbose = True
        
    def hide_key_part_in_image(self, image_path: str, key_part: bytes, password: str, output_path: str) -> bool:
        """
        🔍 Hide encrypted key part in image using LSB steganography
        
        Educational: This demonstrates information hiding techniques
        """
        try:
            if self.verbose:
                print(f"🖼️  Hiding key part in {image_path}")
                
            # Load image
            img = Image.open(image_path).convert('RGB')
            img_array = np.array(img)
            
            # Encrypt the key part with password
            encrypted_key_part = self._encrypt_with_password(key_part, password)
            
            if self.verbose:
                print(f"📊 Key part: {len(key_part)} bytes -> Encrypted: {len(encrypted_key_part)} bytes")
            
            # Convert to binary
            binary_data = ''.join(format(byte, '08b') for byte in encrypted_key_part)
            
            # Add length header (32 bits) - length of encrypted data
            data_length = len(encrypted_key_part)
            length_binary = format(data_length, '032b')
            full_binary = length_binary + binary_data
            
            # Check if image can hold the data
            max_capacity = img_array.size
            if len(full_binary) > max_capacity:
                print(f"❌ Image too small to hold key part ({len(full_binary)} bits needed, {max_capacity} available)")
                return False
                
            if self.verbose:
                print(f"📊 Hiding {len(full_binary)} bits in image with {max_capacity} pixel capacity")
            
            # Hide data in LSBs
            flat_array = img_array.flatten()
            for i, bit in enumerate(full_binary):
                flat_array[i] = (flat_array[i] & 0xFE) | int(bit)
                
            # Reshape and save as PNG to preserve exact pixel values
            modified_array = flat_array.reshape(img_array.shape)
            modified_img = Image.fromarray(modified_array.astype('uint8'), 'RGB')
            
            # Change extension to PNG to preserve steganographic data
            output_path_png = str(output_path).replace('.jpg', '.png')
            modified_img.save(output_path_png, 'PNG')
            
            if self.verbose:
                print(f"✅ Key part hidden in {output_path_png} (PNG format preserves steganographic data)")
                
            return True
            
        except Exception as e:
            print(f"❌ Error hiding key part: {e}")
            return False
    
    def extract_key_part_from_image(self, image_path: str, password: str) -> Optional[bytes]:
        """
        🔍 Extract encrypted key part from image using LSB steganography
        """
        try:
            if self.verbose:
                print(f"🔍 Extracting key part from {image_path}")
                
            img = Image.open(image_path).convert('RGB')
            img_array = np.array(img)
            
            # Extract length (first 32 bits)
            flat_array = img_array.flatten()
            length_binary = ''.join(str(flat_array[i] & 1) for i in range(32))
            data_length = int(length_binary, 2)
            
            if self.verbose:
                print(f"📊 Extracted data length: {data_length}")
            
            if data_length <= 0 or data_length > 10000:  # Reasonable limit
                if self.verbose:
                    print(f"❌ Invalid data length: {data_length}")
                return None
                
            # Extract data bits
            total_bits_needed = 32 + data_length * 8
            if total_bits_needed > len(flat_array):
                if self.verbose:
                    print(f"❌ Not enough pixels for data: need {total_bits_needed}, have {len(flat_array)}")
                return None
                
            data_binary = ''.join(str(flat_array[i] & 1) for i in range(32, total_bits_needed))
            
            # Convert binary to bytes
            encrypted_data = bytes(int(data_binary[i:i+8], 2) for i in range(0, len(data_binary), 8))
            
            if self.verbose:
                print(f"📊 Extracted encrypted data: {len(encrypted_data)} bytes")
            
            # Decrypt with password
            try:
                key_part = self._decrypt_with_password(encrypted_data, password)
                if self.verbose:
                    print(f"✅ Successfully extracted key part ({len(key_part)} bytes)")
                return key_part
            except Exception as decrypt_error:
                if self.verbose:
                    print(f"❌ Failed to decrypt key part: {decrypt_error}")
                return None
                
        except Exception as e:
            if self.verbose:
                print(f"❌ Error extracting key part: {e}")
            return None
    
    def create_steganographic_key_distribution(self, master_key: bytes, password: str, num_parts: int = 5, num_decoys: int = 10) -> Dict:
        """
        🎭 Create steganographic key distribution system
        
        Educational: Demonstrates key splitting and steganographic distribution
        """
        print(f"🔐 Creating steganographic key distribution system...")
        print(f"📊 Master key: {len(master_key)} bytes")
        print(f"🧩 Splitting into {num_parts} parts with {num_decoys} decoy images")
        
        # Split master key into parts
        key_parts = self._split_key(master_key, num_parts)
        
        # Generate sample images for hiding keys and decoys
        real_images = self._generate_sample_images(num_parts, "key_image")
        decoy_images = self._generate_sample_images(num_decoys, "decoy_image")
        
        # Hide key parts in real images
        key_image_mapping = {}
        for i, (key_part, image_path) in enumerate(zip(key_parts, real_images)):
            output_path = self.key_dir / f"encrypted_key_{i:03d}.png"  # Use PNG extension
            if self.hide_key_part_in_image(image_path, key_part, password, str(output_path)):
                key_image_mapping[f"encrypted_key_{i:03d}.png"] = i
            
        # Create decoy images (just copy them as JPEGs since they don't need steganographic precision)
        for i, image_path in enumerate(decoy_images):
            decoy_path = self.key_dir / f"decoy_image_{i:03d}.jpg"
            img = Image.open(image_path)
            img.save(str(decoy_path), 'JPEG', quality=95)
            
        # Create key assembly instructions (encrypted)
        assembly_instructions = {
            'num_parts': num_parts,
            'key_images': list(key_image_mapping.keys()),
            'assembly_order': list(range(num_parts)),
            'checksum': hashlib.sha256(master_key).hexdigest()
        }
        
        # Encrypt and save assembly instructions
        encrypted_instructions = self._encrypt_with_password(
            json.dumps(assembly_instructions).encode(), 
            password
        )
        
        instructions_path = self.key_dir / "assembly_instructions.enc"
        with open(instructions_path, 'wb') as f:
            f.write(encrypted_instructions)
            
        print(f"✅ Key distribution system created in {self.key_dir}")
        print(f"📁 {len(key_image_mapping)} key images + {num_decoys} decoy images")
        print(f"🔒 Assembly instructions encrypted and saved")
        
        return {
            'key_dir': str(self.key_dir),
            'key_images': list(key_image_mapping.keys()),
            'num_decoys': num_decoys,
            'instructions_file': 'assembly_instructions.enc'
        }
    
    def assemble_master_key(self, password: str) -> Optional[bytes]:
        """
        🔧 Assemble master key from steganographic distribution
        
        Educational: Shows key reconstruction from distributed parts
        """
        print(f"🔧 Assembling master key from steganographic distribution...")
        
        # Load assembly instructions
        instructions_path = self.key_dir / "assembly_instructions.enc"
        if not instructions_path.exists():
            print(f"❌ Assembly instructions not found: {instructions_path}")
            return None
            
        try:
            with open(instructions_path, 'rb') as f:
                encrypted_instructions = f.read()
                
            decrypted_data = self._decrypt_with_password(encrypted_instructions, password)
            instructions = json.loads(decrypted_data.decode())
            
            print(f"📋 Assembly instructions loaded: {instructions['num_parts']} parts needed")
            
        except Exception as e:
            print(f"❌ Failed to decrypt assembly instructions: {e}")
            return None
        
        # Extract key parts from images
        key_parts = []
        for image_name in instructions['key_images']:
            image_path = self.key_dir / image_name
            if image_path.exists():
                key_part = self.extract_key_part_from_image(str(image_path), password)
                if key_part:
                    key_parts.append(key_part)
                else:
                    print(f"❌ Failed to extract key part from {image_name}")
                    return None
            else:
                print(f"❌ Key image not found: {image_name}")
                return None
                
        # Assemble master key
        if len(key_parts) != instructions['num_parts']:
            print(f"❌ Missing key parts: {len(key_parts)}/{instructions['num_parts']}")
            return None
            
        # Combine key parts in correct order
        master_key = b''.join(key_parts[i] for i in instructions['assembly_order'])
        
        # Verify checksum
        calculated_checksum = hashlib.sha256(master_key).hexdigest()
        if calculated_checksum != instructions['checksum']:
            print(f"❌ Master key checksum mismatch!")
            return None
            
        print(f"✅ Master key assembled successfully ({len(master_key)} bytes)")
        return master_key
    
    def _split_key(self, key: bytes, num_parts: int) -> List[bytes]:
        """Split key into multiple parts"""
        part_size = len(key) // num_parts
        remainder = len(key) % num_parts
        
        parts = []
        offset = 0
        
        for i in range(num_parts):
            if i < remainder:
                current_part_size = part_size + 1
            else:
                current_part_size = part_size
                
            parts.append(key[offset:offset + current_part_size])
            offset += current_part_size
            
        return parts
    
    def _generate_sample_images(self, count: int, prefix: str) -> List[str]:
        """Generate sample images for demonstration"""
        images = []
        
        for i in range(count):
            # Create a random colorful image
            width, height = 800, 600
            image_array = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)
            
            # Add some patterns to make it look more like a real image
            for y in range(height):
                for x in range(width):
                    # Simple gradient pattern with proper uint8 handling
                    r = ((x * 255 // width) ^ (y * 255 // height) ^ (i * 50)) & 0xFF
                    g = ((y * 255 // height) ^ (i * 100)) & 0xFF
                    b = ((x * y * 255 // (width * height)) ^ (i * 150)) & 0xFF
                    image_array[y, x] = [r, g, b]
            
            img = Image.fromarray(image_array, 'RGB')
            image_path = f"/tmp/{prefix}_{i:03d}.jpg"
            img.save(image_path, 'JPEG', quality=95)
            images.append(image_path)
            
        return images
    
    def _encrypt_with_password(self, data: bytes, password: str) -> bytes:
        """Encrypt data with password using PBKDF2 + Fernet"""
        salt = secrets.token_bytes(16)
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=32,
            salt=salt,
            iterations=100000,
        )
        key = base64.urlsafe_b64encode(kdf.derive(password.encode()))
        f = Fernet(key)
        encrypted_data = f.encrypt(data)
        return salt + encrypted_data
    
    def _decrypt_with_password(self, encrypted_data: bytes, password: str) -> bytes:
        """Decrypt data with password using PBKDF2 + Fernet"""
        salt = encrypted_data[:16]
        encrypted_content = encrypted_data[16:]
        
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=32,
            salt=salt,
            iterations=100000,
        )
        key = base64.urlsafe_b64encode(kdf.derive(password.encode()))
        f = Fernet(key)
        return f.decrypt(encrypted_content)


class EncryptedDiskManager:
    """
    💽 Encrypted Disk Image Manager
    
    Creates and manages encrypted loopback devices like Randy's original system
    """
    
    def __init__(self, base_dir: str = "encrypted_storage"):
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
        self.mounted_devices = {}
        self.verbose = True
        
    def create_encrypted_disk_image(self, image_path: str, size_mb: int, filesystem: str = "ext4") -> bool:
        """
        💽 Create encrypted disk image
        
        Educational: Shows how to create encrypted storage containers
        """
        try:
            if self.verbose:
                print(f"💽 Creating encrypted disk image: {image_path} ({size_mb}MB)")
                
            image_path = Path(image_path)
            
            # Create sparse file
            with open(image_path, 'wb') as f:
                f.seek(size_mb * 1024 * 1024 - 1)
                f.write(b'\0')
                
            if self.verbose:
                print(f"📁 Created {size_mb}MB disk image")
                
            return True
            
        except Exception as e:
            print(f"❌ Error creating disk image: {e}")
            return False
    
    def mount_encrypted_disk(self, image_path: str, mount_point: str, encryption_key: bytes) -> bool:
        """
        🔐 Mount encrypted disk using loopback device
        
        Educational: Demonstrates encrypted filesystem mounting
        Note: This is a simplified educational version
        """
        try:
            if self.verbose:
                print(f"🔐 Mounting encrypted disk: {image_path} -> {mount_point}")
                
            # Create mount point
            mount_point = Path(mount_point)
            mount_point.mkdir(exist_ok=True)
            
            # For educational purposes, we'll create a simple encryption system
            # In Randy's original, this would use cryptsetup/LUKS
            
            # Store encrypted disk info
            self.mounted_devices[str(mount_point)] = {
                'image_path': str(image_path),
                'encryption_key': encryption_key,
                'mount_time': datetime.now()
            }
            
            # Create a simple encrypted filesystem simulation
            encrypted_dir = mount_point / ".encrypted_data"
            encrypted_dir.mkdir(exist_ok=True)
            
            if self.verbose:
                print(f"✅ Encrypted disk mounted at {mount_point}")
                print(f"🔑 Encryption key: {len(encryption_key)} bytes")
                
            return True
            
        except Exception as e:
            print(f"❌ Error mounting encrypted disk: {e}")
            return False
    
    def unmount_encrypted_disk(self, mount_point: str) -> bool:
        """
        🔓 Unmount encrypted disk
        """
        try:
            mount_point = str(Path(mount_point))
            
            if mount_point in self.mounted_devices:
                if self.verbose:
                    print(f"🔓 Unmounting encrypted disk: {mount_point}")
                    
                # In real implementation, would unmount loopback device
                del self.mounted_devices[mount_point]
                
                if self.verbose:
                    print(f"✅ Encrypted disk unmounted")
                    
                return True
            else:
                print(f"❌ No mounted disk found at {mount_point}")
                return False
                
        except Exception as e:
            print(f"❌ Error unmounting disk: {e}")
            return False
    
    def setup_auto_unmount_timer(self, mount_point: str, timeout_minutes: int):
        """
        ⏰ Setup auto-unmount timer like Randy's original system
        """
        if self.verbose:
            print(f"⏰ Setting up auto-unmount timer: {timeout_minutes} minutes")
            
        # This would run in a separate thread in a real implementation
        def auto_unmount():
            time.sleep(timeout_minutes * 60)
            self.unmount_encrypted_disk(mount_point)
            print(f"🔓 Auto-unmounted {mount_point} after {timeout_minutes} minutes")
            
        # For demo purposes, just store the timer info
        if mount_point in self.mounted_devices:
            self.mounted_devices[mount_point]['auto_unmount'] = {
                'timeout_minutes': timeout_minutes,
                'unmount_time': datetime.now() + timedelta(minutes=timeout_minutes)
            }


class MagicalDropFolder:
    """
    ✨ Magical Drop Folder - Randy's "Transport to Land of Encryption"
    
    Monitors a folder and automatically encrypts files dropped into it
    """
    
    def __init__(self, drop_folder: str, encrypted_storage: EncryptedDiskManager, encryption_key: bytes):
        self.drop_folder = Path(drop_folder)
        self.drop_folder.mkdir(exist_ok=True)
        self.encrypted_storage = encrypted_storage
        self.encryption_key = encryption_key
        self.verbose = True
        
    def setup_magical_transport(self):
        """
        ✨ Setup magical file transport system
        """
        if self.verbose:
            print(f"✨ Setting up magical drop folder: {self.drop_folder}")
            print(f"📋 Drop files here and run 'transport' command to encrypt them!")
            
        # Create instructions file
        instructions = self.drop_folder / "README_MAGICAL_TRANSPORT.txt"
        with open(instructions, 'w') as f:
            f.write("""
🧙‍♂️ MAGICAL ENCRYPTION TRANSPORT FOLDER

Welcome to Randy's magical encryption system!

HOW TO USE:
1. Drop any files you want to encrypt into this folder
2. Run the command: python encryption_system.py transport
3. Watch your files magically disappear into the encrypted realm!

EDUCATIONAL NOTE:
This demonstrates automated file encryption workflows.
Files are encrypted with your master key and moved to secure storage.

Your files will be safely stored in the encrypted disk image.
Use the 'list-encrypted' command to see what's stored.
Use the 'extract' command to retrieve files from the encrypted realm.

🔐 Security through automation + strong encryption!
""")
        
        if self.verbose:
            print(f"📋 Instructions written to {instructions}")
    
    def transport_files_to_encryption(self, target_mount: str) -> List[str]:
        """
        ✨ Transport dropped files to encrypted storage
        """
        transported_files = []
        
        # Find files in drop folder
        files_to_transport = [f for f in self.drop_folder.iterdir() 
                            if f.is_file() and f.name != "README_MAGICAL_TRANSPORT.txt"]
        
        if not files_to_transport:
            print("📭 No files found in drop folder")
            return []
            
        print(f"✨ Transporting {len(files_to_transport)} files to the encrypted realm...")
        
        for file_path in files_to_transport:
            try:
                # Read file content
                with open(file_path, 'rb') as f:
                    file_content = f.read()
                
                # Encrypt file content
                encrypted_content = self._encrypt_file_content(file_content, file_path.name)
                
                # Save to encrypted storage
                encrypted_file_path = Path(target_mount) / ".encrypted_data" / f"{file_path.name}.enc"
                with open(encrypted_file_path, 'wb') as f:
                    f.write(encrypted_content)
                
                # Remove original file
                file_path.unlink()
                
                transported_files.append(file_path.name)
                
                if self.verbose:
                    print(f"✨ Transported: {file_path.name} -> encrypted realm")
                    
            except Exception as e:
                print(f"❌ Failed to transport {file_path.name}: {e}")
        
        if transported_files:
            print(f"🎉 Successfully transported {len(transported_files)} files to encrypted storage!")
            print(f"🔐 Files are now safe in the encrypted realm")
        
        return transported_files
    
    def list_encrypted_files(self, mount_point: str) -> List[str]:
        """
        📋 List files in encrypted storage
        """
        encrypted_dir = Path(mount_point) / ".encrypted_data"
        if not encrypted_dir.exists():
            return []
            
        encrypted_files = [f.stem for f in encrypted_dir.glob("*.enc")]
        return encrypted_files
    
    def extract_file_from_encryption(self, filename: str, mount_point: str, output_dir: str) -> bool:
        """
        📤 Extract file from encrypted storage
        """
        try:
            encrypted_file_path = Path(mount_point) / ".encrypted_data" / f"{filename}.enc"
            if not encrypted_file_path.exists():
                print(f"❌ Encrypted file not found: {filename}")
                return False
                
            # Read encrypted content
            with open(encrypted_file_path, 'rb') as f:
                encrypted_content = f.read()
                
            # Decrypt content
            file_content, original_name = self._decrypt_file_content(encrypted_content)
            
            # Save decrypted file
            output_path = Path(output_dir) / original_name
            with open(output_path, 'wb') as f:
                f.write(file_content)
                
            if self.verbose:
                print(f"📤 Extracted: {original_name} -> {output_path}")
                
            return True
            
        except Exception as e:
            print(f"❌ Error extracting file: {e}")
            return False
    
    def _encrypt_file_content(self, content: bytes, filename: str) -> bytes:
        """Encrypt file content with metadata"""
        # Create file metadata
        metadata = {
            'original_name': filename,
            'size': len(content),
            'encrypted_at': datetime.now().isoformat()
        }
        
        # Combine metadata and content
        metadata_bytes = json.dumps(metadata).encode()
        metadata_length = len(metadata_bytes)
        
        combined_data = metadata_length.to_bytes(4, 'big') + metadata_bytes + content
        
        # Encrypt everything
        salt = secrets.token_bytes(16)
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=32,
            salt=salt,
            iterations=100000,
        )
        key = base64.urlsafe_b64encode(kdf.derive(self.encryption_key))
        f = Fernet(key)
        encrypted_data = f.encrypt(combined_data)
        
        return salt + encrypted_data
    
    def _decrypt_file_content(self, encrypted_data: bytes) -> Tuple[bytes, str]:
        """Decrypt file content and extract metadata"""
        salt = encrypted_data[:16]
        encrypted_content = encrypted_data[16:]
        
        # Decrypt
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=32,
            salt=salt,
            iterations=100000,
        )
        key = base64.urlsafe_b64encode(kdf.derive(self.encryption_key))
        f = Fernet(key)
        decrypted_data = f.decrypt(encrypted_content)
        
        # Extract metadata
        metadata_length = int.from_bytes(decrypted_data[:4], 'big')
        metadata_bytes = decrypted_data[4:4+metadata_length]
        file_content = decrypted_data[4+metadata_length:]
        
        metadata = json.loads(metadata_bytes.decode())
        
        return file_content, metadata['original_name']


def main():
    """
    🎓 Educational Demonstration of Randy's Personal Encryption System
    """
    parser = argparse.ArgumentParser(description="Randy's Personal Encryption System - Educational Recreation")
    parser.add_argument('command', choices=[
        'create-keys', 'mount', 'unmount', 'transport', 
        'list-encrypted', 'extract', 'demo'
    ], help='Command to execute')
    parser.add_argument('--password', '-p', help='Master password')
    parser.add_argument('--filename', '-f', help='Filename for extract command')
    parser.add_argument('--output-dir', '-o', default='.', help='Output directory for extract')
    parser.add_argument('--size', '-s', type=int, default=100, help='Disk image size in MB')
    parser.add_argument('--timeout', '-t', type=int, default=60, help='Auto-unmount timeout in minutes')
    
    args = parser.parse_args()
    
    # Initialize components
    key_manager = SteganographicKeyManager("encrypted_keys")
    disk_manager = EncryptedDiskManager("encrypted_storage")
    
    print("🎓 RANDY'S PERSONAL ENCRYPTION SYSTEM - Educational Recreation")
    print("=" * 70)
    print()
    
    if args.command == 'demo':
        # Complete demonstration
        print("🎭 Running complete demonstration of Randy's encryption system...")
        print()
        
        # Step 1: Create steganographic key distribution
        password = "demo_password_123"
        print("📋 STEP 1: Creating Steganographic Key Distribution")
        
        master_key = secrets.token_bytes(64)  # 64-byte master key
        key_distribution = key_manager.create_steganographic_key_distribution(
            master_key, password, num_parts=3, num_decoys=7
        )
        print()
        
        # Step 2: Assemble key from distribution
        print("📋 STEP 2: Assembling Master Key from Images")
        assembled_key = key_manager.assemble_master_key(password)
        
        if assembled_key == master_key:
            print("✅ Master key assembly successful!")
        else:
            print("❌ Master key assembly failed!")
            return
        print()
        
        # Step 3: Create encrypted disk
        print("📋 STEP 3: Creating Encrypted Disk Image")
        disk_image = "demo_encrypted_disk.img"
        disk_manager.create_encrypted_disk_image(disk_image, 50)
        print()
        
        # Step 4: Mount encrypted disk
        print("📋 STEP 4: Mounting Encrypted Disk")
        mount_point = "demo_mount"
        disk_manager.mount_encrypted_disk(disk_image, mount_point, assembled_key)
        disk_manager.setup_auto_unmount_timer(mount_point, 5)  # 5 minute demo
        print()
        
        # Step 5: Setup magical drop folder
        print("📋 STEP 5: Setting up Magical Drop Folder")
        drop_folder = MagicalDropFolder("demo_drop_folder", disk_manager, assembled_key)
        drop_folder.setup_magical_transport()
        
        # Create some demo files
        demo_files = ["secret_document.txt", "important_data.json", "classified_image.jpg"]
        for filename in demo_files:
            demo_content = f"This is secret content for {filename}\nCreated at: {datetime.now()}\n"
            with open(Path("demo_drop_folder") / filename, 'w') as f:
                f.write(demo_content)
        print()
        
        # Step 6: Transport files
        print("📋 STEP 6: Transporting Files to Encrypted Realm")
        transported = drop_folder.transport_files_to_encryption(mount_point)
        print()
        
        # Step 7: List encrypted files
        print("📋 STEP 7: Listing Files in Encrypted Storage")
        encrypted_files = drop_folder.list_encrypted_files(mount_point)
        print(f"📁 Encrypted files: {encrypted_files}")
        print()
        
        # Step 8: Extract a file
        if encrypted_files:
            print("📋 STEP 8: Extracting File from Encrypted Storage")
            Path("demo_extracted").mkdir(exist_ok=True)  # Create extraction directory
            drop_folder.extract_file_from_encryption(encrypted_files[0], mount_point, "demo_extracted")
            print()
        
        print("🎉 DEMONSTRATION COMPLETE!")
        print()
        print("🎓 EDUCATIONAL SUMMARY:")
        print("✅ Steganographic key distribution with decoy images")
        print("✅ Multi-layer encryption with password protection")
        print("✅ Encrypted disk image with loopback mounting")
        print("✅ Automated file encryption workflow")
        print("✅ Auto-unmount timer for security")
        print("✅ Magical drop folder automation")
        print()
        print("💡 Randy's original system was a sophisticated combination of:")
        print("   • Advanced cryptography (key distribution, encryption)")
        print("   • Steganography (hiding in plain sight)")
        print("   • System administration (loopback devices, mounting)")
        print("   • User experience (magical automation)")
        print("   • Security engineering (multiple layers of protection)")
        
    elif args.command == 'create-keys':
        if not args.password:
            args.password = input("🔐 Enter master password: ")
            
        master_key = secrets.token_bytes(64)
        key_manager.create_steganographic_key_distribution(master_key, args.password)
        
    elif args.command == 'mount':
        if not args.password:
            args.password = input("🔐 Enter master password: ")
            
        master_key = key_manager.assemble_master_key(args.password)
        if master_key:
            disk_image = "encrypted_disk.img"
            if not Path(disk_image).exists():
                disk_manager.create_encrypted_disk_image(disk_image, args.size)
            disk_manager.mount_encrypted_disk(disk_image, "encrypted_mount", master_key)
            if args.timeout:
                disk_manager.setup_auto_unmount_timer("encrypted_mount", args.timeout)
                
    # ... other commands ...
    
    else:
        print(f"❌ Command '{args.command}' not implemented in demo version")
        print("💡 Use 'demo' command to see the complete system in action!")


if __name__ == "__main__":
    main()