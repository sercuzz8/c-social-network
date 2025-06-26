# redemption-arc
Fixing my old university project to use proper data structures

# The project
Implementation in standard C language (with only libc) of a programme that reads from standard input a sequence of commands, each corresponding to a change in entities or relationships between entities and, when required, produces on standard output, for each type of relationship monitored, the identifier of the entity that is the recipient of the greatest number of instances of that relationship, and the number of relationships that entity receives

## Commands

- addent ⟨id_ent⟩: adds an entity identified by “id_ent” to the set of monitored entities; if the entity is already monitored, it does nothing
- delent ⟨id_ent⟩: deletes the entity identified by “id_ent” from the set of monitored entities; deletes all relationships to which “id_ent” belongs (either as source or destination)
- addrel ⟨id_orig⟩ ⟨id_dest⟩ ⟨id_ent⟩: adds a relation - identified by “id_rel” - between the entities “id_orig” and “id_dest”, where “id_dest” is the receiver of the relation. If the relationship between “id_orig” and “id_dest” already exists, or if at least one of the entities is not monitored, it does nothing. Monitoring of the relation type “id_rel” implicitly starts with the first “addrel” command concerning it.
- delrel ⟨id_orig⟩ ⟨id_dest⟩ ⟨id_ent⟩: eliminates the relationship identified by “id_rel” between the entities “id_orig” and “id_dest” (where “id_dest” is the receiver of the relationship); if there is no “id_rel” relationship between “id_orig” and “id_dest” (with “id_dest” as receiver), it does nothing
- report: outputs the list of relations, reporting for each the entities with the highest number of incoming relations, as explained below
- end: end of the sequence of commands

## Additional Information

- Identifiers (both entity and relation) are always enclosed in “”.
- Assume also that each identifier may contain only letters (upper or lower case), digits, and the symbols “_” and “-”.
- there is no need to check that the received identifiers respect this
convention, it can be taken for granted
- All identifiers (of both entities and relations) are "case sensitive", so "Alice" and "alice" are different identifiers
- The output of the report command is a sequence made as follows: ⟨id_rel1⟩ ⟨id_ent1⟩ ⟨n_rel1⟩; ⟨id_rel2⟩ ⟨id_ent2 ⟨n_rel2⟩; ...
- the output relations are sorted in ascending order of identifier
- if for a relation type there are more entities that are recipients of the maximum number of relations, these are output in ascending order of identifier, e.g.: ⟨id_rel1⟩ ⟨id_ent1_1⟩ ⟨id_ent1_2⟩ ⟨id_ent1_3⟩ ... ⟨n_rel1⟩ ;
- if all relations with a certain identifier are removed, it does not appear in the subsequent outputs of the report command
- if there are no relations between entities, the output is “none” (without inverted commas)
- The ordering of identifiers follows the ASCII character table, so the following order applies: - < 1 < A < _ < a
- The various parts of each command and each output sequence are separated by spaces
- The end command has no output

## Tests

Every test is contained in the eponymous folder. The input files are the **.in** ones and the output ones have the **.out** suffix. The simplest one is the **0.0**.