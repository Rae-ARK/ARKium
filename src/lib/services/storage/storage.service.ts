export class StorageService {
	async readJson<T>(path: string): Promise<T | null> {
		try {
			const response = await fetch(path);

			if (!response.ok) {
				return null;
			}

			return await response.json();
		} catch {
			return null;
		}
	}

	async writeJson(_path: string, _data: unknown): Promise<void> {
		// Platform implementations:
		// Android -> FileProvider
		// Desktop -> Tauri fs
	}

	async loadLibrary() {
		return this.readJson('/data/metadata.json');
	}

	async saveLibrary(data: unknown) {
		return this.writeJson('/data/metadata.json', data);
	}
}

export const storageService = new StorageService();
