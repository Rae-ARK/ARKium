import { fileProvider } from '$lib/services/file';
import { library } from '$lib/stores/library.svelte';
import type { LibraryEntry } from '$lib/models';

export class ImportService {
	async chooseLibraryFolder() {
		const folder = await fileProvider.pickFolder();

		const entry: LibraryEntry = {
			id: crypto.randomUUID(),
			title: folder.name,
			path: folder.id,
			progress: 0,
			addedAt: new Date().toISOString()
		};

		library.set([entry]);

		return entry;
	}
}

export const importService = new ImportService();
