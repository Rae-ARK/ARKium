import { Capacitor } from '@capacitor/core';
import { FileProviderNative } from './file.provider.plugin';
import type { FileProvider, FileEntry, FolderPermission } from '../file.types';

class AndroidFileProvider implements FileProvider {
	async pickFolder(): Promise<FolderPermission> {
		if (!Capacitor.isNativePlatform()) {
			throw new Error('Android FileProvider requires native platform');
		}

		const result = await FileProviderNative.pickFolder();

		return {
			id: result.folderId,
			name: 'Selected folder',
			granted: true
		};
	}

	async listFiles(folderId: string): Promise<FileEntry[]> {
		if (!Capacitor.isNativePlatform()) {
			throw new Error('Android FileProvider requires native platform');
		}

		const result = await FileProviderNative.listFiles({ folderId });

		return result.files.map((file) => ({
			name: file.name,
			path: file.path,
			type: file.type,
			size: file.size
		}));
	}

	async readFile(path: string): Promise<string> {
		if (!Capacitor.isNativePlatform()) {
			throw new Error('Android FileProvider requires native platform');
		}

		const result = await FileProviderNative.readFile({ path });
		return result.content;
	}
}

export const androidFileProvider = new AndroidFileProvider();
