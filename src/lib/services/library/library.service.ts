import type { LibraryEntry } from '$lib/models';
import { storageService } from '$lib/services/storage';

const LIBRARY_PATH = '/data/metadata.json';

export class LibraryService {
	async load(): Promise<LibraryEntry[]> {
		return (await storageService.readJson<LibraryEntry[]>(LIBRARY_PATH)) ?? [];
	}

	async save(items: LibraryEntry[]): Promise<void> {
		await storageService.writeJson(LIBRARY_PATH, items);
	}
}

export const libraryService = new LibraryService();
