<script lang="ts">
	import { resolve } from '$app/paths';
	import { library } from '$lib/stores/library.svelte';
</script>

<section>
	<h1>Library</h1>

	{#if library.status === 'loading'}
		<div class="state">
			<p>Loading your library…</p>
		</div>
	{:else if library.status === 'error'}
		<div class="state state-error">
			<h2>Something went wrong</h2>
			<p>{library.error}</p>
		</div>
	{:else if library.items.length === 0}
		<div class="state">
			<h2>No novels yet</h2>
			<p>Choose a library folder to begin.</p>
			<a class="cta" href={resolve('/import')}>Import novels</a>
		</div>
	{:else}
		<ul class="entries">
			{#each library.items as item (item.id)}
				<li class="entry">
					<span class="title">{item.title}</span>
					{#if item.author}
						<span class="author">{item.author}</span>
					{/if}
				</li>
			{/each}
		</ul>
	{/if}
</section>

<style>
	section {
		display: flex;
		flex-direction: column;
		gap: 1rem;
	}

	.state {
		padding: 2rem;
		border-radius: 16px;
		background: var(--surface);
		color: var(--text);
	}

	.state-error {
		border: 1px solid var(--error, #b3261e);
	}

	.cta {
		display: inline-block;
		margin-top: 1rem;
		color: var(--primary, inherit);
		font-weight: 600;
	}

	.entries {
		display: flex;
		flex-direction: column;
		gap: 0.5rem;
		list-style: none;
		margin: 0;
		padding: 0;
	}

	.entry {
		display: flex;
		flex-direction: column;
		padding: 1rem;
		border-radius: 16px;
		background: var(--surface);
	}

	.title {
		font-weight: 600;
		color: var(--text);
	}

	.author {
		font-size: 0.875rem;
		color: var(--text-secondary);
	}

	h1,
	h2 {
		margin: 0;
	}

	p {
		margin: 0.5rem 0 0;
		color: var(--text-secondary);
	}
</style>
