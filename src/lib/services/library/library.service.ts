import type { LibraryEntry } from '$lib/models';
import { storageService } from '$lib/services/storage';

export class LibraryService {
	async load(): Promise<LibraryEntry[]> {
		return (await storageService.loadLibrary<LibraryEntry[]>()) ?? [];
	}

	async save(items: LibraryEntry[]): Promise<void> {
		await storageService.saveLibrary(items);
	}

	async add(item: LibraryEntry, items: LibraryEntry[]): Promise<LibraryEntry[]> {
		const updated = [...items, item];
		await this.save(updated);
		return updated;
	}

	async remove(id: string, items: LibraryEntry[]): Promise<LibraryEntry[]> {
		const updated = items.filter((entry) => entry.id !== id);
		await this.save(updated);
		return updated;
	}
}

export const libraryService = new LibraryService();
