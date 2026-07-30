import type { FileProvider } from './file.types';

class UnimplementedFileProvider implements FileProvider {
	async pickFolder(): Promise<never> {
		throw new Error('FileProvider not implemented');
	}

	async listFiles(folderId: string): Promise<never> {
		void folderId;
		throw new Error('FileProvider not implemented');
	}

	async readFile(path: string): Promise<never> {
		void path;
		throw new Error('FileProvider not implemented');
	}
}

export const fileProvider: FileProvider = new UnimplementedFileProvider();
