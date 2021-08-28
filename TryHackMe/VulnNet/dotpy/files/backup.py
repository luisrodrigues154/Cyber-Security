from datetime import datetime
from pathlib import Path
import zipfile


OBJECT_TO_BACKUP = '/home/manage'  # The file or directory to backup
BACKUP_DIRECTORY = '/var/backups'  # The location to store the backups in
MAX_BACKUP_AMOUNT = 300  # The maximum amount of backups to have in BACKUP_DIRECTORY


object_to_backup_path = Path(OBJECT_TO_BACKUP)
backup_directory_path = Path(BACKUP_DIRECTORY)
assert object_to_backup_path.exists()  # Validate the object we are about to backup exists before we continue

# Validate the backup directory exists and create if required
backup_directory_path.mkdir(parents=True, exist_ok=True)

# Get the amount of past backup zips in the backup directory already
existing_backups = [
    x for x in backup_directory_path.iterdir()
    if x.is_file() and x.suffix == '.zip' and x.name.startswith('backup-')
]

# Enforce max backups and delete oldest if there will be too many after the new backup
oldest_to_newest_backup_by_name = list(sorted(existing_backups, key=lambda f: f.name))
while len(oldest_to_newest_backup_by_name) >= MAX_BACKUP_AMOUNT:  # >= because we will have another soon
    backup_to_delete = oldest_to_newest_backup_by_name.pop(0)
    backup_to_delete.unlink()

# Create zip file (for both file and folder options)
backup_file_name = f'backup-{datetime.now().strftime("%Y%m%d%H%M%S")}-{object_to_backup_path.name}.zip'
zip_file = zipfile.ZipFile(str(backup_directory_path / backup_file_name), mode='w')
if object_to_backup_path.is_file():
    # If the object to write is a file, write the file
    zip_file.write(
        object_to_backup_path.absolute(),
        arcname=object_to_backup_path.name,
        compress_type=zipfile.ZIP_DEFLATED
    )
elif object_to_backup_path.is_dir():
    # If the object to write is a directory, write all the files
    for file in object_to_backup_path.glob('**/*'):
        if file.is_file():
            zip_file.write(
                file.absolute(),
                arcname=str(file.relative_to(object_to_backup_path)),
                compress_type=zipfile.ZIP_DEFLATED
            )
# Close the created zip file
zip_file.close()

