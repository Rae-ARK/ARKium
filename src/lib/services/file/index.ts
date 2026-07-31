import { Capacitor } from '@capacitor/core';
import { androidFileProvider } from './android/android.file.provider';
import { fileProvider as unimplementedFileProvider } from './file.provider';
import type { FileProvider } from './file.types';

function resolveFileProvider(): FileProvider {
	if (Capacitor.getPlatform() === 'android') {
		return androidFileProvider;
	}

	// Desktop (Tauri) implementation lands in a future milestone.
	return unimplementedFileProvider;
}

export const fileProvider: FileProvider = resolveFileProvider();

export type { FileEntry, FileProvider, FolderPermission } from './file.types';
