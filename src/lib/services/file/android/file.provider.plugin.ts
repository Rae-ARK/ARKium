import { registerPlugin } from '@capacitor/core';

export interface FolderResult {
	folderId: string;
}

export interface NativeFileEntry {
	name: string;
	path: string;
	type: 'file' | 'directory';
	size: number;
}

export interface ListFilesResult {
	files: NativeFileEntry[];
}

export interface ReadFileResult {
	content: string;
}

export interface FileProviderPlugin {
	pickFolder(): Promise<FolderResult>;
	listFiles(options: { folderId: string }): Promise<ListFilesResult>;
	readFile(options: { path: string }): Promise<ReadFileResult>;
}

export const FileProviderNative = registerPlugin<FileProviderPlugin>('FileProvider');
