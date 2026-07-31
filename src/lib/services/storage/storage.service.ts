import { Capacitor } from '@capacitor/core';
import { Directory, Encoding, Filesystem } from '@capacitor/filesystem';

const LIBRARY_FILE = 'metadata.json';

/**
 * Web/dev fallback so storage works in the browser during development.
 * Not persisted across page reloads. Native builds always use Filesystem.
 * A durable desktop implementation (Tauri fs) lands in a future milestone.
 */
class MemoryFallbackStore {
	private store = new Map<string, string>();

	read(path: string): string | null {
		return this.store.get(path) ?? null;
	}

	write(path: string, contents: string): void {
		this.store.set(path, contents);
	}
}

const memoryFallback = new MemoryFallbackStore();

export class StorageService {
	private isNative(): boolean {
		return Capacitor.isNativePlatform();
	}

	async readJson<T>(path: string): Promise<T | null> {
		try {
			const raw = this.isNative()
				? (
						await Filesystem.readFile({
							path,
							directory: Directory.Data,
							encoding: Encoding.UTF8
						})
					).data
				: memoryFallback.read(path);

			if (typeof raw !== 'string' || raw.length === 0) {
				return null;
			}

			return JSON.parse(raw) as T;
		} catch {
			// Missing file on first run is expected, not an error.
			return null;
		}
	}

	async writeJson(path: string, data: unknown): Promise<void> {
		const contents = JSON.stringify(data, null, 2);

		if (this.isNative()) {
			await Filesystem.writeFile({
				path,
				directory: Directory.Data,
				encoding: Encoding.UTF8,
				data: contents
			});
		} else {
			memoryFallback.write(path, contents);
		}
	}

	async loadLibrary<T>(): Promise<T | null> {
		return this.readJson<T>(LIBRARY_FILE);
	}

	async saveLibrary(data: unknown): Promise<void> {
		return this.writeJson(LIBRARY_FILE, data);
	}
}

export const storageService = new StorageService();
