import { registerPlugin } from '@capacitor/core';

export interface FolderResult {
	folderId: string;
}

export interface FileProviderPlugin {
	pickFolder(): Promise<FolderResult>;

	listFiles(options: { folderId: string }): Promise<unknown>;

	readFile(options: { path: string }): Promise<unknown>;
}

export const FileProviderNative = registerPlugin<FileProviderPlugin>('FileProvider');
