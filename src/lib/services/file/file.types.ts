export interface FileEntry {
	name: string;
	path: string;
	type: 'file' | 'directory';
	size?: number;
}

export interface FolderPermission {
	id: string;
	name: string;
	granted: boolean;
}

export interface FileProvider {
	pickFolder(): Promise<FolderPermission>;

	listFiles(folderId: string): Promise<FileEntry[]>;

	readFile(path: string): Promise<string>;
}
