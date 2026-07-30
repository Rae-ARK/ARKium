import { Capacitor } from '@capacitor/core';

import { FileProviderNative } from './file.provider.plugin';

import type { FileProvider } from '../file.types';
import type { FolderPermission } from '../file.types';

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

	async listFiles(folderId: string): Promise<never> {
		if (!Capacitor.isNativePlatform()) {
			throw new Error('Android FileProvider requires native platform');
		}

		void folderId;

		throw new Error('Android SAF not implemented');
	}

	async readFile(path: string): Promise<never> {
		if (!Capacitor.isNativePlatform()) {
			throw new Error('Android FileProvider requires native platform');
		}

		void path;

		throw new Error('Android SAF not implemented');
	}
}

export const androidFileProvider = new AndroidFileProvider();
