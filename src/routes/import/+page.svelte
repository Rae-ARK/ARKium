<script lang="ts">
	import { importService } from '$lib/services/import';
	import { library } from '$lib/stores/library.svelte';

	let status = $state<'idle' | 'importing' | 'error'>('idle');
	let errorMessage = $state('');
	let lastImportedCount = $state<number | null>(null);

	async function chooseFolder() {
		status = 'importing';
		errorMessage = '';
		lastImportedCount = null;

		try {
			const newEntries = await importService.chooseLibraryFolder();
			lastImportedCount = newEntries.length;
			status = 'idle';
		} catch (err) {
			console.error(err);
			errorMessage = err instanceof Error ? err.message : 'Failed to import folder.';
			status = 'error';
		}
	}
</script>

<h1>Import</h1>

<button onclick={chooseFolder} disabled={status === 'importing'}>
	{status === 'importing' ? 'Importing…' : 'Choose Library Folder'}
</button>

{#if status === 'error'}
	<p class="error">{errorMessage}</p>
{/if}

{#if lastImportedCount !== null}
	<p>
		{lastImportedCount === 0
			? 'No new TXT files found in that folder.'
			: `Added ${lastImportedCount} novel${lastImportedCount === 1 ? '' : 's'} to your library.`}
	</p>
{/if}

{#if library.items.length}
	<h2>Library</h2>

	<ul>
		{#each library.items as item (item.id)}
			<li>{item.title}</li>
		{/each}
	</ul>
{/if}

<style>
	.error {
		color: var(--error, #b3261e);
	}
</style>
