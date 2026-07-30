import type { LibraryEntry } from '$lib/models';

class LibraryStore {
	items = $state<LibraryEntry[]>([]);

	set(items: LibraryEntry[]) {
		this.items = items;
	}

	add(item: LibraryEntry) {
		this.items = [...this.items, item];
	}

	clear() {
		this.items = [];
	}
}

export const library = new LibraryStore();
